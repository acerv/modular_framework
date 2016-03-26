/*
 * config_window.h
 *
 * Copyright (C) 2014 - Andrea Cervesato <sawk.ita@gmail.com>
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIG_WINDOW_H
#define CONFIG_WINDOW_H

/* the config window */
struct _ConfigWindow;
typedef struct _ConfigWindow ConfigWindow;

/* create the config window */
ConfigWindow* config_window_new(void* parent);

/* destroy the config window */
void config_window_destroy(ConfigWindow* window);

/* add config tab control */
void config_window_add_tab(ConfigWindow* window, void* tabcontrol, const char* tabname);

/* remove config tab control */
void config_window_remove_tab(ConfigWindow* window, const char* tabname);

/* show config tab control */
void config_window_show_tab(ConfigWindow* window, const char* tabname); 

/* show the config control */
void config_window_show(ConfigWindow* window);

#endif
