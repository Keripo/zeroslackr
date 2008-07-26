/*
 * Last updated: July 26, 2008
 * ~Keripo
 *
 * Copyright (C) 2008 Keripo
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

#include "browser-ext.h"

static PzModule *module;

static PzWindow *fastlaunch()
{
	pz_exec_kill("/opt/Zillae/Podzilla0-Lite/Launch/Launch.sh");
	return NULL;
}

static PzWindow *readme()
{
	return new_textview_window(
		"/opt/Zillae/Podzilla0-Lite/ReadMe from Keripo.txt");
}

static void init_launch() 
{
	module = pz_register_module("Podzilla0-Lite", 0);
	pz_menu_add_stub_group("/Zillae/Podzilla0-Lite", "PZ0 Based");
	pz_menu_add_action_group("/Zillae/Podzilla0-Lite/#FastLaunch", "#FastLaunch", fastlaunch);
	pz_menu_add_action_group("/Zillae/Podzilla0-Lite/~ReadMe", "#FastLaunch", readme);
	pz_menu_sort("/Zillae/Podzilla0-Lite");
}

PZ_MOD_INIT (init_launch)
