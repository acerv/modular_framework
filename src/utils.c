/*
 * utils.c
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

#include "utils.h"

const char* const* g_hash_table_return_keys_to_array(GHashTable* table, unsigned int** size) {
    GList* keys = NULL;
    GList* item = NULL;
    const char** names = NULL;
    const char* name = NULL;
    size_t i = 0;
    unsigned int length = 0;

    g_return_val_if_fail(table != NULL, NULL);
   
    /* get keys into list format */ 
    keys = g_hash_table_get_keys(table);
    length = g_list_length(keys);
    
    /* convert list to array of strings */
    names = g_malloc(length * sizeof(char*));

    for (i = 0; i < length; i++) {
        item = g_list_nth(keys, i);
        name = (char*)(item->data);
        names[i] = g_strdup(name);
    }

    /* assign the array length */
    **size = length;
    
    return names;
}

