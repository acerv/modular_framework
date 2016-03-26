/*
 * engine.h
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

#ifndef ENGINE_H
#define ENGINE_H

#include "module.h"

/* Abstract data type that rapresents the Engine */
struct _Engine;
typedef struct _Engine Engine;

/* Engine modes */
enum {
    NORMAL_MODE,
    DEMO_MODE,
    DEBUG_MODE
};

/* it loads the engine with the specified mode */
Engine* engine_new(GraphicControls* controls, int mode, 
    const char* configPath, const char* setupPath, const char* modulesDir);

/* it closes the engine */
void engine_free(Engine* engine);

/* it returns true if the engine is in demo mode */
int engine_is_demo(Engine* engine);

/* it returns true if the engine is in debug mode */
int engine_is_debug(Engine* engine);

/* configuration/setup/modules path */
void engine_set_config_path(Engine* engine, const char* path);
void engine_set_setup_path(Engine* engine, const char* path);
void engine_set_modules_dir(Engine* engine, const char* directory);

const char* engine_get_config_path(Engine* engine);
const char* engine_get_setup_path(Engine* engine);
const char* engine_get_modules_dir(Engine* engine);

/* it returns the number of modules */
unsigned int engine_get_modules_num(Engine* engine);

/* it returns the modules' names */
const char* const* engine_get_modules_names(Engine* engine, unsigned int* size);

#endif
