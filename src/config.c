/*
 * config.c
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

#include <glib.h>
#include "config.h"
#include "definitions.h"
#include "utils.h"

/* Define the abstract data type */
struct _CfgFile {
    const char* file;
    const char* name;
    GHashTable* data;
};

static CfgFile* cfg_mask_new(const char* file, const char* name, GHashTable* data) {
    CfgFile* cfgPtr = NULL;

    g_return_if_fail(file != NULL);
    g_return_if_fail(name != NULL);
    g_return_if_fail(data != NULL);

    cfgPtr = g_new(CfgFile, 1);

    cfgPtr->file = g_strdup(file);
    cfgPtr->name = g_strdup(name);
    cfgPtr->data = data;

    return cfgPtr;
}

static void cfg_mask_store(const char* file, const char* cfgname, GHashTable* cfg) {
    GKeyFile* keyFile = NULL;
    GError* error = NULL;
    GHashTableIter iter;
    void* key = NULL;
    void* value = NULL;
    const char* datafile = NULL;
    unsigned long length = 0;

    g_return_if_fail(STRING_IS_VALID(file));
    g_return_if_fail(STRING_IS_VALID(cfgname));
    g_return_if_fail(cfg != NULL);
    g_return_if_fail(g_file_test(file, G_FILE_TEST_EXISTS));
    g_return_if_fail(g_file_test(file, G_FILE_TEST_IS_REGULAR));

    /* try to open the key-value file */
    keyFile = g_key_file_new();

    if (g_key_file_load_from_file(keyFile, file, G_KEY_FILE_NONE, &error) == FALSE) {
        print_error(error);
        return;
    }

   /* store key-value from cfg */ 
    g_hash_table_iter_init(&iter, cfg);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        g_key_file_set_value(keyFile, cfgname, key, value);
    }

    /* save the file */
    datafile = g_key_file_to_data(keyFile, &length, &error);
    if (error) {
        print_error(error);
        return;
    }

    if (g_file_set_contents(file, datafile, length, &error) == FALSE) {
        print_error(error);
        return;
    }

    /* free up some memory */
    g_key_file_free(keyFile);
}

static GHashTable* cfg_mask_load(const char* file, const char* cfgname) {
    GHashTable* cfg = NULL;
    GKeyFile* keyFile = NULL;
    void* key = NULL;
    void* value = NULL;
    char** keys = NULL;
    GError* error = NULL;
    unsigned long length = 0;
    size_t i = 0;

    g_return_if_fail(STRING_IS_VALID(file));
    g_return_if_fail(STRING_IS_VALID(cfgname));

    /* try to open the key-value file */
    keyFile = g_key_file_new();

    if (g_key_file_load_from_file(keyFile, file, G_KEY_FILE_NONE, &error) == FALSE) {
        print_error(error);
        return NULL;
    }

    g_return_if_fail(g_key_file_has_group(keyFile, cfgname) == TRUE);

    /* create the hash table */
    cfg = g_hash_table_new(g_str_hash, g_str_equal);

    /* get the keys associated to the configuration name */
    keys = g_key_file_get_keys(keyFile, cfgname, &length, &error);
    if (error) {
        print_error(error);
        return NULL;
    }

    for (i = 0; i < length; i++) {
        key = STRING_TO_POINTER(g_strdup(keys[i]));
        value = g_key_file_get_value(keyFile, cfgname, key, &error);
        if (error) {
            print_error(error);
            continue;
        }

        /* populate the hash table */
        g_hash_table_insert(cfg, key, value);
    }

    /* free up some memory */
    g_key_file_free(keyFile);

    return cfg;
}

/* Implementations */
CfgFile* cfg_new(const char* file, const char* name) {
    return cfg_mask_new(file, name, g_hash_table_new(g_str_hash, g_str_equal));
}

void cfg_free(CfgFile* cfg) {
    g_return_if_fail(cfg != NULL);

    g_assert(cfg->file != NULL);
    g_assert(cfg->name != NULL);
    g_assert(cfg->data != NULL);

    g_hash_table_destroy(cfg->data);

    cfg->file = NULL;
    cfg->name = NULL;
    cfg->data = NULL;

    g_free(cfg);

    cfg = NULL;
}

void cfg_store(CfgFile* cfg) {
    g_return_if_fail(cfg != NULL);

    cfg_mask_store(cfg->file, cfg->name, cfg->data);
}

CfgFile* cfg_load(const char* file, const char* name) {
    return cfg_mask_new(file, name, cfg_mask_load(file, name));
}

void cfg_add_item(CfgFile* cfg, const char* key, const char* value) {
    g_return_if_fail(cfg != NULL);
    g_return_if_fail(key != NULL);
    g_return_if_fail(value != NULL);

    g_hash_table_insert(cfg->data, STRING_TO_POINTER(key), STRING_TO_POINTER(value));
}

const char* cfg_get_item_value(CfgFile* cfg, const char* key) {
    void* valuePtr = NULL;
    char* value = NULL;

    g_return_val_if_fail(cfg != NULL, NULL);
    g_return_val_if_fail(key != NULL, NULL);

    value = NULL;

    if (g_hash_table_contains(cfg->data, POINTER_TO_STRING(key)) == FALSE)
        return NULL;

    valuePtr = g_hash_table_lookup(cfg->data, POINTER_TO_STRING(key));

    g_assert(valuePtr != NULL);

    value = g_strdup(POINTER_TO_STRING(valuePtr));

    return value;
}

unsigned int cfg_get_size(CfgFile* cfg) {
    g_return_val_if_fail(cfg != NULL, NULL);
    
    return g_hash_table_size(cfg->data);
}

const char* const* cfg_get_keys(CfgFile* cfg, unsigned int* size) {
    g_return_val_if_fail(cfg != NULL, NULL);
    
    return g_hash_table_return_keys_to_array(cfg->data, &size);
}

