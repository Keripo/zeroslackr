/*
 * Copyright (c) 2005 Joshua Oreman, Bernard Leach, and Courtney Cavin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

// KERIPO MOD
#include "_mods.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h> /*d*/
#include "pz.h"
#ifdef IPOD
#include "ucdl.h"
#endif

const char *PZ_Developers[] = {
    "Bernard Leach",
    "Joshua Oreman",
    "David Carne",
    "Courtney Cavin",
    "Scott Lawrence",
    "James Jacobsson",
    "Adam Johnston",
    "Alastair Stuart",
    "Jonathynne Bettencourt",
    0
};

FILE *errout;

void ____Spurious_references_to_otherwise_unreferenced_symbols() 
{
    TWidget *(*tnivw)(int,int,ttk_surface) = ttk_new_imgview_widget; (void) tnivw;
    /* Add anything else *in TTK only* that's unrefed and
     * needed by a module.
     */

#ifndef IPOD
    /* And now for the div funcs and such in libgcc...
     * rand() is so the compiler can't do constant
     * optimization.
     */
    unsigned long ul = 1337 + rand();
    unsigned long long ull = 42424242 + rand();
    long l = ul; long long ll = ull;
    float f = 3.14159 + (float)rand();
    double d = 2.718281828 + (float)rand();

#define DO_STUFF_INT(x) { x += 5; x *= 27; x %= 420; x /= 42; x <<= 4; x -= 3; x >>= 20; }
#define DO_STUFF_FP(x) { x += 5.0; x *= 27.0; x /= 42.0; x -= 3.0; }
    DO_STUFF_INT(ul); DO_STUFF_INT(ull);
    DO_STUFF_INT(l);  DO_STUFF_INT(ll);
    DO_STUFF_FP(f);   DO_STUFF_FP(d);
#endif
}

/* compat globals */
t_GR_SCREEN_INFO screen_info;
long hw_version;
ttk_gc pz_root_gc;

/* static stuff */
static ttk_timer connection_timer = 0;
static int bl_forced_on = 0;

/* Is something connected? */
int usb_connected = 0;
int fw_connected = 0;

/* Set to tell ipod.c not to actually set the wheel debounce,
 * since we don't want it set *while we're setting it*.
 */
int pz_setting_debounce = 0;

/* Is hold on? */
int pz_hold_is_on = 0;

/* The global config. */
PzConfig *pz_global_config;

static void check_connection() 
{
    int this_usb_conn = pz_ipod_usb_is_connected();
    int this_fw_conn  = pz_ipod_fw_is_connected();
    int show_popup    = pz_get_int_setting (pz_global_config, USB_FW_POPUP);

    if (show_popup && this_usb_conn && !usb_connected &&
        pz_dialog (_("USB Connect"), _("Go to disk mode?"), 2, 10, "No", "Yes"))
        pz_ipod_go_to_diskmode();

    if( pz_ipod_get_hw_version() < 0x000B0000 ) {
        if (show_popup && this_fw_conn && !fw_connected &&
            pz_dialog (_("FireWire Connect"), _("Go to disk mode?"), 2, 10, "No", "Yes"))
            pz_ipod_go_to_diskmode();
    }

    usb_connected = this_usb_conn;
    fw_connected = this_fw_conn;
    connection_timer = ttk_create_timer (1000, check_connection);
}

int usb_fw_connected()
{
    return !!(usb_connected + fw_connected);
}

static ttk_timer bloff_timer = 0;

static void backlight_off() { if (!bl_forced_on) pz_ipod_set (BACKLIGHT, 0); bloff_timer = 0; }
static void backlight_on()  { pz_ipod_set (BACKLIGHT, 1); }

void pz_set_backlight_timer (int sec) 
{
    static int last = PZ_BL_OFF;
    if (sec != PZ_BL_RESET) last = sec;

    if (last == PZ_BL_OFF) {
	if (bloff_timer) ttk_destroy_timer (bloff_timer);
	bloff_timer = 0;
	backlight_off();
    } else if (last == PZ_BL_ON) {
	if (bloff_timer) ttk_destroy_timer (bloff_timer);
	bloff_timer = 0;
	backlight_on();
    } else {
	if (bloff_timer) ttk_destroy_timer (bloff_timer);
	bloff_timer = ttk_create_timer (1000*last, backlight_off);
	backlight_on();
    }
}

void backlight_toggle() 
{
    bl_forced_on = !bl_forced_on;
    pz_handled_hold ('m');
}

static int held_times[128] = { ['m'] = 500 }; // key => ms
static void (*held_handlers[128])() = { ['m'] = backlight_toggle };
static int (*unused_handlers[128])(int, int);

static int held_ignores[128]; // set a char to 1 = ignore its UP event once.
static ttk_timer held_timers[128]; // the timers

void pz_register_global_hold_button (unsigned char ch, int ms, void (*handler)()) 
{
    held_times[ch] = ms;
    held_handlers[ch] = handler;
}
void pz_unregister_global_hold_button (unsigned char ch) 
{
    held_times[ch] = 0; held_handlers[ch] = 0;
}

void pz_register_global_unused_handler (unsigned char ch, int (*handler)(int, int)) 
{
    unused_handlers[ch] = handler;
}
void pz_unregister_global_unused_handler (unsigned char ch) 
{
    unused_handlers[ch] = 0;
}

void pz_handled_hold (unsigned char ch)
{
    held_timers[ch] = 0;
    held_ignores[ch]++;
}

int pz_event_handler (int ev, int earg, int time)
{
    static int vtswitched = 0;

    pz_set_backlight_timer (PZ_BL_RESET);
    pz_reset_idle_timer();

    /* unset setting_debounce if we're not anymore */
    if (pz_setting_debounce && (ttk_windows->w->focus->draw != ttk_slider_draw)) {
	pz_setting_debounce = 0;
	pz_ipod_set (WHEEL_DEBOUNCE, pz_get_int_setting (pz_global_config, WHEEL_DEBOUNCE));
    }

    switch (ev) {
    case TTK_BUTTON_DOWN:
	switch (earg) {
	case TTK_BUTTON_HOLD:
	    pz_hold_is_on = 1;
	    pz_header_fix_hold();
	    break;
	case TTK_BUTTON_MENU:
	    vtswitched = 0;
	    break;
	}
	if (held_times[earg] && held_handlers[earg]) {
	    if (held_timers[earg]) ttk_destroy_timer (held_timers[earg]);
	    held_timers[earg] = ttk_create_timer (held_times[earg], held_handlers[earg]);
	}
	break;
    case TTK_BUTTON_UP:
	if (held_timers[earg]) {
	    ttk_destroy_timer (held_timers[earg]);
	    held_timers[earg] = 0;
	}
	if (held_ignores[earg]) {
	    held_ignores[earg] = 0;
	    return 1;
	}
	switch (earg) {
	case TTK_BUTTON_HOLD:
	    pz_hold_is_on = 0;
	    pz_header_fix_hold();
	    break;
	case TTK_BUTTON_PREVIOUS:
	    if (pz_get_int_setting (pz_global_config, ENABLE_VTSWITCH) &&
		ttk_button_pressed (TTK_BUTTON_MENU) && ttk_button_pressed (TTK_BUTTON_PLAY)) {
		// vt switch code <<
		printf ("VT SWITCH <<\n");
		vtswitched = 1;
		return 1;
	    } else if (pz_get_int_setting (pz_global_config, ENABLE_VTSWITCH) &&
		       ttk_button_pressed (TTK_BUTTON_MENU) && ttk_button_pressed (TTK_BUTTON_NEXT)) {
		// vt switch code [0]
		printf ("VT SWITCH 0 (N-P)\n");
		vtswitched = 1;
		return 1;
	    } else if (pz_get_int_setting (pz_global_config, ENABLE_WINDOWMGMT) &&
                       ttk_button_pressed (TTK_BUTTON_MENU) && !vtswitched) {
		TWindowStack *lastwin = ttk_windows;
		while (lastwin->next) lastwin = lastwin->next;
		if (lastwin->w != ttk_windows->w) {
		    ttk_move_window (lastwin->w, 0, TTK_MOVE_ABS);
		    printf ("WINDOW CYCLE >>\n");
		} else
		    printf ("WINDOW CYCLE >> DIDN'T\n");
		return 1;
	    }
	    break;
	case TTK_BUTTON_NEXT:
	    if (pz_get_int_setting (pz_global_config, ENABLE_VTSWITCH) &&
		ttk_button_pressed (TTK_BUTTON_MENU) && ttk_button_pressed (TTK_BUTTON_PLAY)) {
		// vt switch code >>
		printf ("VT SWITCH >>\n");
		vtswitched = 1;
		return 1;
	    } else if (pz_get_int_setting (pz_global_config, ENABLE_VTSWITCH) &&
		       ttk_button_pressed (TTK_BUTTON_MENU) && ttk_button_pressed (TTK_BUTTON_PREVIOUS)) {
		// vt switch code [0]
		printf ("VT SWITCH 0 (P-N)\n");
		vtswitched = 1;
		return 1;
	    } else if (pz_get_int_setting (pz_global_config, ENABLE_WINDOWMGMT) &&
                       ttk_button_pressed (TTK_BUTTON_MENU) && !vtswitched) {
		printf ("WINDOW CYCLE <<\n");
		if (ttk_windows->next) {
		    ttk_move_window (ttk_windows->w, 0, TTK_MOVE_END);
		    return 1;
		}
	    }
	    break;
	case TTK_BUTTON_PLAY:
	    if (pz_get_int_setting (pz_global_config, ENABLE_WINDOWMGMT) &&
                ttk_button_pressed (TTK_BUTTON_MENU) && !vtswitched) {
		printf ("WINDOW MINIMIZE\n");
		if (ttk_windows->next) {
		    ttk_windows->minimized = 1;
		    return 1;
		}
	    }
	}
	break;
    }
    return 0; // keep event
}


int pz_unused_handler (int ev, int earg, int time) 
{
    switch (ev) {
    case TTK_BUTTON_UP:
	if (unused_handlers[earg])
	    return unused_handlers[earg](earg, time);
	break;
    }
    return 0;
}


void pz_set_time_from_file(void)
{
#ifdef IPOD
	struct timeval tv_s;
	struct stat statbuf;

	/* find the last modified time of the settings file */
// KERIPO MOD
//	stat( "/etc/podzilla/podzilla.conf", &statbuf );
	stat( CONFIG_FILE, &statbuf );

	/* convert timespec to timeval */
	tv_s.tv_sec  = statbuf.st_mtime;
	tv_s.tv_usec = 0;

	settimeofday( &tv_s, NULL );
#endif
}

void pz_touch_settings(void) 
{
#ifdef IPOD
// KERIPO MOD
//	close (open ("/etc/podzilla/podzilla.conf", O_WRONLY));
	close (open (CONFIG_FILE, O_WRONLY));
#endif
}


void pz_uninit() 
{
	ttk_quit();
	pz_touch_settings();
	pz_modules_cleanup();
        fclose (errout);
}

#ifdef IPOD
void
decode_instr (FILE *f, unsigned long *iaddr)
{
    unsigned long instr = *iaddr;
    fprintf (f, "%8lx:\t%08lx\t", (unsigned long)iaddr, instr);
    if ((instr & 0xff000000) == 0xeb000000) {
        unsigned long field = instr & 0xffffff;
        fprintf (f, "bl\tf=%lx", field);
        unsigned long uoffs = field << 2;
        if (uoffs & (1 << 25)) uoffs |= 0xfc000000;
        long offs = uoffs;
        fprintf (f, "  o=%lx", offs);
        unsigned long addr = (unsigned long)iaddr + 8 + offs;
        fprintf (f, "  a=%08lx", addr);
        const char *modfile, *symname;
        symname = uCdl_resolve_addr (addr, &offs, &modfile);
        fprintf (f, " <%s+%lx> from %s\n", symname, offs, modfile);
    } else {
        fprintf (f, "???\n");
    }
}

void
debug_handler (int sig) 
{
    unsigned long PC, *FP;
    unsigned long retaddr, off;
    const char *modfile = "Unknown";
    int i;
// KERIPO MOD
//    FILE *f = fopen ("podzilla.oops", "w");
    FILE *f = fopen (PZOOPS, "w");

    asm ("mov %0, r11" : "=r" (FP) : );

    /* arm_fp field of struct sigcontext is 0x3c bytes
     * above our FP. The regs are in order, so PC is
     * 0x10 bytes above FP.
     */
    PC = *(unsigned long  *)((char *)FP + 0x4c);
    FP = *(unsigned long **)((char *)FP + 0x3c);
    fprintf (f, "FP = 0x%lx\n", (unsigned long)FP);

    ttk_quit();
    fprintf (stderr, "Fatal signal %d\n", sig);
    fprintf (stderr, "Trying to gather debug info. If this freezes, reboot.\n\n");

    const char *func = uCdl_resolve_addr (PC, &off, &modfile);
    fprintf (f, "#0  %08lx <%s+%lx> from %s\n", PC, func, off, modfile);
    decode_instr (f, (unsigned long *)PC - 3);

    for (i = 1; i < 10; i++) {
        retaddr = *(FP - 1);
        fprintf (f, "#%d  %08lx <%s+%lx> from %s\n", i, retaddr, uCdl_resolve_addr (retaddr, &off, &modfile), off, modfile);
        decode_instr (f, (unsigned long *)retaddr - 1);
        FP = (unsigned long *) *(FP - 3);
    }
    fclose (f);
// KERIPO MOD
//    fprintf (stderr, "Saved: podzilla.oops\n");
    fprintf (stderr, "Saved: " PZOOPS "\n");
    pz_touch_settings();
    pz_modules_cleanup();
    fprintf (stderr, "Letting original sig go - expect crash\n");
    signal (sig, SIG_DFL);
    return;
}
#endif

void
usage( char * exename )
{
	fprintf( stderr, "Usage: %s [options...]\n", exename );
	fprintf( stderr, "Options:\n" );
	fprintf( stderr, "  -g gen    set simulated ipod generation.\n" );
	fprintf( stderr, "            \"gen\" can be one of:\n" );
	fprintf( stderr, "                1g, 2g, 3g, 4g, 5g, scroll, touch,\n" );
	fprintf( stderr, "                dock, mini, photo, color, nano, video\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "  -2  W H   use a screen W by H, 2bpp (monochrome)\n" );
	fprintf( stderr, "  -16 W H   use a screen W by H, 16bpp (color)\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "default resolution and color are for 1g-4g mono iPod\n" );
}


int
main(int argc, char **argv)
{
	TWindow *first;
	int initialContrast = ipod_get_contrast();
	if( initialContrast < 1 ) initialContrast = 96;

        if (!TTK_VERSION_CHECK()) {
        	fprintf (stderr, "Version mismatch; exiting.\n");
        	return 1;
        }

//#ifdef IPOD
        signal (SIGBUS, debug_handler);

// KERIPO MOD
// Defined in "mods.h"
// #define SCHEMESDIR "/usr/share/schemes/"
// #else
// #define SCHEMESDIR "schemes/"
// #endif

        errout = stderr;

	if (argc > 1) {
		if (argv[1][0] == '-') {
			/* predefined sizes */
			if (!strcmp (argv[1], "-g")) {
				if( !strcmp( argv[2], "1g" ))	  ttk_set_emulation( 160, 128, 2 );
				if( !strcmp( argv[2], "2g" ))	  ttk_set_emulation( 160, 128, 2 );
				if( !strcmp( argv[2], "3g" ))	  ttk_set_emulation( 160, 128, 2 );
				if( !strcmp( argv[2], "4g" ))	  ttk_set_emulation( 160, 128, 2 );
				if( !strcmp( argv[2], "5g" ))	  ttk_set_emulation( 320, 240, 16 );

				if( !strcmp( argv[2], "scroll" )) ttk_set_emulation( 160, 128, 2 );
				if( !strcmp( argv[2], "touch" ))  ttk_set_emulation( 160, 128, 2 );
				if( !strcmp( argv[2], "dock" ))   ttk_set_emulation( 160, 128, 2 );
				if( !strcmp( argv[2], "mini" ))	  ttk_set_emulation( 138, 110, 2 );

				if( !strcmp( argv[2], "photo" ))  ttk_set_emulation( 220, 176, 16 );
				if( !strcmp( argv[2], "color" ))  ttk_set_emulation( 220, 176, 16 );
				if( !strcmp( argv[2], "nano" ))	  ttk_set_emulation( 176, 132, 16 );
				if( !strcmp( argv[2], "video" ))  ttk_set_emulation( 320, 240, 16 );

			/* remove these four eventually... */
			} else if (!strcmp (argv[1], "-photo")) {
				fprintf( stderr, "ERROR: \"-photo\" is deprecated, use \"-g photo\" instead.\n" );
				exit( -42 );
			} else if (!strcmp (argv[1], "-nano")) {
				fprintf( stderr, "ERROR: \"-nano\" is deprecated, use \"-g nano\" instead.\n" );
				exit( -42 );
			} else if (!strcmp (argv[1], "-mini")) {
				fprintf( stderr, "ERROR: \"-mini\" is deprecated, use \"-g mini\" instead.\n" );
				exit( -42 );
			} else if (!strcmp (argv[1], "-video")) {
				fprintf( stderr, "ERROR: \"-video\" is deprecated, use \"-g video\" instead.\n" );
				exit( -42 );

			/* arbitrary size... */
			} else if (!strcmp (argv[1], "-2")) {
				if( argc != 4 ) {
					usage( argv[0] );
					exit( -2 );
				}
				ttk_set_emulation ( atoi( argv[2] ),
						    atoi( argv[3] ), 2 );
			} else if (!strcmp (argv[1], "-16")) {
				if( argc != 4 ) {
					usage( argv[0] );
					exit( -2 );
				}
				ttk_set_emulation ( atoi( argv[2] ),
						    atoi( argv[3] ), 16 );

                        } else if (!strcmp (argv[1], "-errout")) {
                                if( argc != 3 ) {
                                        usage( argv[0] );
                                        exit( -2 );
                                }
                                errout = fopen (argv[2], "a");
			} else {
				usage( argv[0] );
				exit( -1 );
			}
		}
	}

// KERIPO MOD
//         if (access (SCHEMESDIR "default.cs", R_OK) < 0)
//             symlink ("mono.cs", SCHEMESDIR "default.cs");
        if (access (SYMLINKSCHEMEDIR SYMLINKDEFSCHEME, R_OK) < 0)
            symlink (SCHEMESDIR DEFSCHEME, SYMLINKSCHEMEDIR SYMLINKDEFSCHEME);

	if ((first = ttk_init()) == 0) {
		fprintf(stderr, _("ttk_init failed\n"));
		exit(1);
	}
	ttk_hide_window (first);
	atexit (pz_uninit);

#ifdef IPOD
        char exepath[256];
        if (argv[0][0] == '/')
            strcpy (exepath, argv[0]);
        else
            sprintf (exepath, "/bin/%s", argv[0]);

	if (uCdl_init (exepath) == 0) {
		ttk_quit();
		fprintf (stderr, _("uCdl_init failed: %s\n"), uCdl_error());
		exit (0);
	}
#endif

	ttk_set_global_event_handler (pz_event_handler);
	ttk_set_global_unused_handler (pz_unused_handler);

#ifdef LOCALE
	setlocale(LC_ALL, "");
// KERIPO EDIT
// Eh, why not.
// 	bindtextdomain("podzilla", LOCALEDIR);
// 	textdomain("podzilla");
	bindtextdomain(PZNAME, LOCALEDIR);
	textdomain(PZNAME);
#endif

	pz_root_gc = ttk_new_gc();
	ttk_gc_set_usebg(pz_root_gc, 0);
	ttk_gc_set_foreground(pz_root_gc, ttk_makecol (0, 0, 0));
	t_GrGetScreenInfo(&screen_info);

	hw_version = pz_ipod_get_hw_version();

	if( hw_version && hw_version < 0x30000 ) { /* 1g/2g only */
		pz_set_time_from_file();
	}

// KERIPO MOD
// Defined in "mods.h"
// #ifdef IPOD
// #define CONFIG_FILE "/etc/podzilla/podzilla.conf"
// #else
// #define CONFIG_FILE "config/podzilla.conf"
// #endif

	pz_global_config = pz_load_config (CONFIG_FILE);
	/* Set some sensible defaults */
#define SET(x) pz_get_setting(pz_global_config,x)
// KERIPO MOD
// Just some customizations
	if (!SET(WHEEL_DEBOUNCE))   pz_ipod_set (WHEEL_DEBOUNCE, 10);
	if (!SET(CONTRAST))         pz_ipod_set (CONTRAST, initialContrast);
//	if (!SET(CLICKER))          pz_ipod_set (CLICKER, 1);
	if (!SET(CLICKER))          pz_ipod_set (CLICKER, 0);
	if (!SET(DSPFREQUENCY))     pz_ipod_set (DSPFREQUENCY, 0);
//	if (!SET(SLIDE_TRANSIT))    pz_ipod_set (SLIDE_TRANSIT, 1);
	if (!SET(SLIDE_TRANSIT))    pz_ipod_set (SLIDE_TRANSIT, 2);
	if (!SET(BACKLIGHT))        pz_ipod_set (BACKLIGHT, 1);
	if (!SET(BACKLIGHT_TIMER))  pz_ipod_set (BACKLIGHT_TIMER, 3);
	if (!SET(COLORSCHEME))      pz_ipod_set (COLORSCHEME, 0);
	pz_save_config (pz_global_config);
	pz_ipod_fix_settings (pz_global_config);
	pz_load_font (&ttk_textfont, "Espy Sans", TEXT_FONT, pz_global_config);
//	pz_load_font (&ttk_menufont, "Chicago",   MENU_FONT, pz_global_config);
	pz_load_font (&ttk_menufont, "Sabine Doscbthm",   MENU_FONT, pz_global_config);
	pz_menu_init();
	pz_modules_init();
	pz_header_init();
// KERIPO MOD
// Modified new ZeroLauncher menus
// Commented out menus do not show by default
// 	pz_menu_sort ("/Extras/Demos");
// 	pz_menu_sort ("/Extras/Games");
// 	pz_menu_sort ("/Extras/Utilities");
// 	pz_menu_sort ("/Extras");

	//pz_menu_sort ("/Beta Testing");
	//pz_menu_sort ("/Custom");
	pz_menu_sort ("/Emulators");
	//pz_menu_sort ("/Emulators/Non-Consoles");
	pz_menu_sort ("/Media");
	//pz_menu_sort ("/Media/Games");
	//pz_menu_sort ("/Media/Demos");
	pz_menu_sort ("/Tools");
	//pz_menu_sort ("/Tools/Dev");
	pz_menu_sort ("/Zillae");
	//pz_menu_sort ("/Zillae/Custom Builds");
	//pz_menu_sort ("/Extras");
    	pz_menuconf_init();
	ttk_show_window (pz_menu_get());

	connection_timer = ttk_create_timer (1000, check_connection);
	usb_connected = pz_ipod_usb_is_connected();
	fw_connected = pz_ipod_fw_is_connected();

	return ttk_run();
}