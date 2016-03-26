/*
 * test_module.h
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

#include "definitions.h"
#include "module.h"

const char* mod_get_name(void) {
    return "Test Module";
}

const char* mod_get_version(void) {
    return "1.0";
}

void mod_setup(GraphicControls* controls, int mode) {
    g_print("Test: mod_setup is called\n\r");
}

void logic_run(void) {
    g_print("Test: logic_run is called\n\r");
}

void logic_close(void) {
    g_print("Test: logic_close is called\n\r");
}

void graphic_run(void) {
    g_print("Test: graphic_run is called\n\r");
}

void graphic_close(void) {
    g_print("Test: graphic_close is called\n\r");
}

void conf_save_config(const char* filepath) {
    g_print("Test: conf_save_config is called\n\r");
}

void conf_load_config(const char* filepath) {
    g_print("Test: conf_load_config is called\n\r");
}

void conf_save_setup(const char* filepath) {
    g_print("Test: conf_save_setup is called\n\r");
}

void conf_load_setup(const char* filepath) {
    g_print("Test: conf_load_setup is called\n\r");
}

void conf_setup_form_closing(int saveRequest) {
    g_print("Test: conf_setup_form_closing is called\n\r");
}

void conf_config_form_closing(int saveRequest) {
    g_print("Test: conf_config_form_closing is called\n\r");
}
