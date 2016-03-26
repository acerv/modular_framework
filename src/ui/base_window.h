/*
 * base_window.h
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

#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include "config_window.h"
#include "test_window.h"

/* the docking placement */
enum {
    DOCK_FLOAT = 0,
    DOCK_LEFT,
    DOCK_RIGHT,
    DOCK_TOP,
    DOCK_BOTTOM,
    DOCK_DOCUMENT
};

/* the base window definition */
struct _BaseWindow;
typedef struct _BaseWindow BaseWindow;

/* create base window */
BaseWindow* base_window_new(ConfigWindow* configForm, ConfigWindow* setupForm, TestWindow* testForm);

/* destroy base window */
void base_window_destroy(BaseWindow* window);

/* show the base window */
void base_window_show(BaseWindow* window);

/* close the base window */
void base_window_close(BaseWindow* window);

/* bring to front base window */
void base_window_bring_to_front(BaseWindow* window);

/* show menu */
void base_window_show_menu(BaseWindow* window, unsigned int show);

/* add menu item */
void base_window_add_menu_item(BaseWindow* window, void* item, unsigned int position);

/* remove menu item */
void base_window_remove_menu_item(BaseWindow* window, unsigned int position);

/* get the menu item */
void* base_window_get_menu_item(BaseWindow* window, unsigned int position);

/* show toolbar */
void base_window_show_toolbar(BaseWindow* window, unsigned int show);

/* add toolbar item */
void base_window_add_toolbar_item(BaseWindow* window, void* item, unsigned int position);

/* remove toolbar item */
void base_window_remove_toolbar_iten(BaseWindow* window, unsigned int position);

/* show information toolbar */
void base_window_show_info_toolbar(BaseWindow* window, unsigned int show);

/* add information toolbar item */
void base_window_add_info_toolbar_item(BaseWindow* window, void* item, unsigned int position);

/* remove information toolbar item */
void base_window_remove_info_toolbar_item(BaseWindow* window, unsigned int position);

/* register a dockable window */
void base_window_register_dockable_control(BaseWindow* window, void* control, unsigned int placement);

/* get the dockable windows which has been added */
const void* const* base_window_get_dockable_controls(BaseWindow* window, unsigned int* size);

/* load docking file */
void base_window_load_docking_file(const char* file);

/* save docking file */
void base_window_save_docking_file(const char* file);

#endif
