/*
 * Last updated: Apr 27, 2008
 * ~Keripo
 *
 * Copyright (C) 2008 Keripo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.7
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

#include "browser-ext.h"

static PzModule *module;
static ttk_menu_item browser_extension;
static const char *path, *dir;

static int check_ext(const char *file)
{
	return check_file_ext(file, ".sbg");
}

static void warning()
{
	pz_message("WARNING! SBaGen is a binaural beat generator and may have adverse effects on your brain!");
	pz_message("Neither the author (Jim Peters) nor I (Keripo) will take any responsibilities in your actions!");
	pz_message("USE AND EXPERIMENT WITH THIS UTILITY AT YOUR OWN RISK!");
}

static PzWindow *load_file(const char *file)
{
	warning();
	const char *const cmd[] = {"Launch.sh", file, NULL};
	pz_execv(
		path,
		(char *const *)cmd
	);
	return NULL;
}

static PzWindow *load_file_handler(ttk_menu_item *item)
{
	return load_file(item->data);
}

static PzWindow *browse_beats()
{
	chdir(dir);
	return open_directory_title(dir, "Binaural Beats");
}

static PzWindow *fastlaunch()
{
	warning();
	pz_exec(path);
	return NULL;
}

static void cleanup()
{
	pz_browser_remove_handler(check_ext);
}

static void init_launch() 
{
	module = pz_register_module("SBaGen", cleanup);
	path = "/opt/Media/SBaGen/Launch/Launch.sh";
	dir = "/opt/Media/SBaGen/Beats";
	
	pz_menu_add_stub_group("/Media/SBaGen", "Music");
	pz_menu_add_action_group("/Media/SBaGen/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Media/SBaGen/Beats", "Browse", browse_beats);
	pz_menu_add_action_group("/Media/SBaGen/Toggle Backlight", "~Settings", toggle_backlight_window);
	pz_menu_sort("/Media/SBaGen");
	
	browser_extension.name = N_("Open with SBaGen");
	browser_extension.makesub = load_file_handler;
	pz_browser_add_action (check_ext, &browser_extension);
	pz_browser_set_handler(check_ext, load_file);
}

PZ_MOD_INIT(init_launch)