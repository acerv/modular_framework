/*
 * data.h
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

#ifndef DATA_H
#define DATA_H

/* Abstract data type that rapresents the data manager */
struct DataHandler_type;
typedef struct DataHandler_type DataHandler;

/* Event handlers */
typedef struct {
    char* name;
    void* data;
    int from_file;
    char* path;
} dmEventArgs;

typedef void (*dmEventFunc)(dmEventArgs* args);
typedef void (*dmClearEventFunc)(void);

/* Generic routines to open and save data files into directories */
typedef void (*openFileFunc)(const char* path, char** name, void** data);
typedef void (*saveFileFunc)(const void* data, const char* path);
typedef int (*cmpDataFunc)(const void* data0, const void* data1);

/* Data manager creation/destruction */
DataHandler* dh_new(openFileFunc openCallback, saveFileFunc saveCallback, cmpDataFunc cmpCallback);
void dh_free(DataHandler* dm);

/* Event handling management */
void dh_add_load_event(DataHandler* dm, dmEventFunc handler);
void dh_add_unload_event(DataHandler* dm, dmEventFunc handler);
void dh_add_save_event(DataHandler* dm, dmEventFunc handler);
void dh_add_clear_event(DataHandler* dm, dmClearEventFunc handler);

/* Data management */
void dh_clear_data(DataHandler* dm);
void dh_load_data(DataHandler* dm, const char* name, void* data);
void dh_unload_data(DataHandler* dm, const char* name);
void dh_open_data(DataHandler* dm, const char* path);
void dh_save_data(DataHandler* dm, const char* name);
void dh_save_data_on_file(DataHandler* dm, const char* name, const char* path);

/* Maximum data settings */
void dh_set_max_data(DataHandler* dm, unsigned int maxdata);
unsigned int dh_get_max_data(DataHandler* dm);

/* Data informations */
unsigned int dh_get_data_size(DataHandler* dm);
const char* const* dh_get_data_names(DataHandler* dm, unsigned int* size);
const char* dh_get_data_name(DataHandler* dm, const void* data);
void* dh_get_data_by_name(DataHandler* dm, const char* name);
int dh_data_is_loaded(DataHandler* dm, const void* data);
int dh_data_is_loaded_from_file(DataHandler* dm, const void* data);

#endif
