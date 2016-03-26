/*
 * config.h
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

#ifndef CONFIG_H
#define CONFIG_H

/* Abstract data type that rapresents the configuration file */
struct _CfgFile;
typedef struct _CfgFile CfgFile;

/* Creates a custom configuration */
CfgFile* cfg_new(const char* file, const char* name);

/* Free up configuration resources */
void cfg_free(CfgFile* cfg);

/* Stores a configuration */
void cfg_store(CfgFile* cfg);

/* Loads a configuration */
CfgFile* cfg_load(const char* file, const char* name);

/* Adds a configuration item */
void cfg_add_item(CfgFile* cfg, const char* key, const char* value);

/* Returns a configuration item value */
const char* cfg_get_item_value(CfgFile* cfg, const char* key);

/* Returns the number of configuration items */
unsigned int cfg_get_size(CfgFile* cfg);

/* Returns the list of configuration keys */
const char* const* cfg_get_keys(CfgFile* cfg, unsigned int* size);

#endif
