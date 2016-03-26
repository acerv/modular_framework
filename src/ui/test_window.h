/*
 * test_window.h
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

#ifndef TEST_WINDOW_H
#define TEST_WINDOW_H

/* the test window */
struct _TestWindow;
typedef struct _TestWindow TestWindow;

/* create test window */
TestWindow* test_window_new(void* parent);

/* destroy test window */
void test_window_destroy(TestWindow* window);

/* add test window tab */
void test_window_add_tab(TestWindow* window, void* tabcontrol, const char* name);

/* remove test window tab */
void test_window_remove_tab(TestWindow* window, const char* name);

/* show the test control */
void test_window_show(TestWindow* window);

#endif
