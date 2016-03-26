/*
 * data.c
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
#include "data.h"
#include "definitions.h"
#include "errors.h"
#include "utils.h"

#define POINTER_TO_DATAINFO( x ) ( (DataInfos*) x )
#define DATAINFO_TO_POINTER( x ) ( (void*) x )

#define MAXIMUM_DATA_BOUND 100
#define MINIMUM_DATA_BOUND 1
#define DEFAULT_MAXIMUM_DATA 30

/* The data handler */
struct _DataHandler {
    /* Maximum data */
    unsigned int maxData;

    /* Loaded data */
    GHashTable* data;

    /* Callbacks */
    openFileFunc openCallback;
    saveFileFunc saveCallback;
    cmpDataFunc cmpCallback;

    /* Event handlers */
    GSList* dataLoadEventHandler;
    GSList* dataUnloadEventHandler;
    GSList* dataSaveEventHandler;
    GSList* dataClearEventHandler;
};

/* errors */
static const char* _maxDataMsg = "Maximum data exceeded (%i).";
static const char* _nameExistsMsg = "'%s' name already exists.";

/* A callback container */
typedef struct {
	dmEventFunc callback;
    voidEventFunc nullCallback;
} CallbackContainer;

/* Data informations container */
typedef struct {
    char* name;        /* The data name */
	void* data;		/* The data */
	int from_file;	/* If true, the data is loaded from file */
	char* path;		/* The data path. NULL if data is not loaded from file */
} DataInfos;

static GError* get_max_data_error(unsigned int maxcount) {
    GError* error = g_error_new(
        g_quark_from_string(_maxDataMsg),
        DATA_ERROR_MAX_DATA_EXCEEDED,
        _maxDataMsg,
        maxcount);

    return error;
}

static GError* get_name_exists_error(const char* name) {
    GError* error = g_error_new(
        g_quark_from_string(_nameExistsMsg),
        DATA_ERROR_NAME_EXISTS,
        _nameExistsMsg,
        name);

    return error;
}

static DataInfos* new_datainfo(const char* name, void* data, const char* path) {
	DataInfos* cdata = NULL;

	g_assert(STRING_IS_VALID(name));
	g_assert(data != NULL);
	
	cdata = g_new0(DataInfos, 1);
    cdata->name = g_strdup(name);
	cdata->data = data;

	if (STRING_IS_VALID(path)) {
		cdata->path = g_strdup(path);
		cdata->from_file = TRUE;
	} else {
		cdata->path = NULL;
		cdata->from_file = FALSE;
	}

	return cdata;
}

static void datainfo_free(DataInfos* datainfo) {
    g_free(datainfo); 
}

static void add_callback(GSList** list, dmEventFunc handler) {
    CallbackContainer* container = NULL;

    g_return_if_fail(handler != NULL);

    container = g_new(CallbackContainer, 1);
    container->nullCallback = NULL;
    container->callback = handler;

    *list = g_slist_append(*list, (void*)container);
}

static void add_null_callback(GSList** list, voidEventFunc handler) {
    CallbackContainer* container = NULL;

    g_return_if_fail(handler != NULL);

    container = g_new(CallbackContainer, 1);
    container->callback = NULL;
    container->nullCallback = handler;

    *list = g_slist_append(*list, (void*)container);
}

static void rise_events(GSList* list, dmEventArgs* args) {
    GSList* item = NULL;
    CallbackContainer* container = NULL;

    item = list;

    while (item) {
        container = (CallbackContainer*)item->data;

        if (args) {
            container->callback(args);
        } else {
            container->nullCallback();
        }

        item = g_slist_next(item);
    }
}

static void mask_load_data(DataHandler* dm, const char* path, const char* name, void* data) {
	DataInfos* cdata = NULL;
    dmEventArgs* args = NULL;

    g_return_if_fail(dm != NULL);

	/* check if the maximum amount of loeaded data has been reached */
	if (dh_get_data_size(dm) > dm->maxData) {
        print_error(get_max_data_error(dm->maxData));
        return;
    }
	
	g_return_if_fail(STRING_IS_VALID(name));
	g_return_if_fail(data != NULL);

	/* check if data name is already in use */
	if (g_hash_table_contains(dm->data, name) == TRUE) {
        print_error(get_name_exists_error(name));
        return;
    }

	/* create data container and append it in the data list */
	cdata = new_datainfo(name, data, path);

	g_hash_table_insert(dm->data, STRING_TO_POINTER(name), DATAINFO_TO_POINTER(cdata));

    /* notify the event */
    if (g_slist_length(dm->dataLoadEventHandler) > 0) {
        args = g_new(dmEventArgs, 1);
        
        args->name = g_strdup(cdata->name);
        args->path = g_strdup(cdata->path);
        args->from_file = cdata->from_file;
        args->data = data;

        rise_events(dm->dataLoadEventHandler, args);
    }
}

/* implementations */
DataHandler* dh_new(openFileFunc openCallback, saveFileFunc saveCallback, cmpDataFunc cmpCallback) {
    DataHandler* dm = NULL;

	g_return_val_if_fail(openCallback != NULL, NULL);
	g_return_val_if_fail(saveCallback != NULL, NULL);
    g_return_val_if_fail(cmpCallback != NULL, NULL);
	
    /* create data manager */
    dm = g_new(DataHandler, 1);
    
    dm->maxData = DEFAULT_MAXIMUM_DATA;
    dm->data = g_hash_table_new(g_str_hash, g_str_equal);
    
    dm->openCallback = openCallback;
    dm->saveCallback = saveCallback;
    dm->cmpCallback = cmpCallback;

    dm->dataLoadEventHandler = NULL;
    dm->dataUnloadEventHandler = NULL;
    dm->dataSaveEventHandler = NULL;
    dm->dataClearEventHandler = NULL;

    return dm;
}

void dh_free(DataHandler* dm) {
    g_return_if_fail(dm != NULL);

    g_hash_table_remove_all(dm->data);

    g_slist_free(dm->dataLoadEventHandler);
    g_slist_free(dm->dataUnloadEventHandler);
    g_slist_free(dm->dataSaveEventHandler);
    g_slist_free(dm->dataClearEventHandler);

    dm->openCallback = NULL;
    dm->saveCallback = NULL;
    dm->cmpCallback = NULL;
    dm->data = NULL;

    g_free(dm);

    dm = NULL;
}

void dh_add_load_event(DataHandler* dm, dmEventFunc handler) {
    g_return_if_fail(dm != NULL);
    g_return_if_fail(handler != NULL);

    add_callback(&dm->dataLoadEventHandler, handler);
}

void dh_add_unload_event(DataHandler* dm, dmEventFunc handler) {
    g_return_if_fail(dm != NULL);
    g_return_if_fail(handler != NULL);

    add_callback(&dm->dataUnloadEventHandler, handler);
}

void dh_add_save_event(DataHandler* dm, dmEventFunc handler) {
    g_return_if_fail(dm != NULL);
    g_return_if_fail(handler != NULL);

    add_callback(&dm->dataSaveEventHandler, handler);
}

void dh_add_clear_event(DataHandler* dm, voidEventFunc handler) {
    g_return_if_fail(dm != NULL);
    g_return_if_fail(handler != NULL);

    add_null_callback(&dm->dataClearEventHandler, handler);
}

unsigned int dh_get_data_size(DataHandler* dm) {
    g_return_val_if_fail(dm != NULL, 0);

	return g_hash_table_size(dm->data);
}

const char* const* dh_get_data_names(DataHandler* dm, unsigned int* size) {
    g_return_val_if_fail(dm != NULL, NULL);

    return g_hash_table_return_keys_to_array(dm->data, &size);
}

void dh_set_max_data(DataHandler* dm, unsigned int maxdata) {
    g_return_if_fail(dm != NULL);
    
    if (maxdata < MINIMUM_DATA_BOUND)
        dm->maxData = MINIMUM_DATA_BOUND;
    else if (maxdata > MAXIMUM_DATA_BOUND)
        dm->maxData = MAXIMUM_DATA_BOUND;
    else
        dm->maxData = maxdata;
}

unsigned int dh_get_max_data(DataHandler* dm) {
    g_return_val_if_fail(dm != NULL, 0);

    return dm->maxData;
}

void dh_load_data(DataHandler* dm, const char* name, void* data) {
    mask_load_data(dm, NULL, name, data);
}

void dh_unload_data(DataHandler* dm, const char* name) {
    dmEventArgs* args = NULL;
    void* value = NULL;
    DataInfos* cdata = NULL;

    g_return_if_fail(dm != NULL);
	g_return_if_fail(STRING_IS_VALID(name));

	/* find the data by name */
    value = g_hash_table_lookup(dm->data, name);

    g_return_if_fail(value != NULL);

    /* remove data */
    g_hash_table_remove(dm->data, name);	

    /* notify the event */
    if (g_slist_length(dm->dataUnloadEventHandler) > 0) {
        cdata = POINTER_TO_DATAINFO(value);

        args = g_new(dmEventArgs, 1);

        args->name = g_strdup(cdata->name);
        args->path = g_strdup(cdata->path);
        args->from_file = cdata->from_file;
        args->data = cdata->data;

        rise_events(dm->dataUnloadEventHandler, args);
    }

    /* free up resources */
    datainfo_free(cdata);
}

void dh_open_data(DataHandler* dm, const char* path) {
	void* p = g_new0(void*, 1);
	char* name = g_new0(char, 1);

    g_return_val_if_fail(dm != NULL, NULL);
	g_return_if_fail(STRING_IS_VALID(path));
	
	dm->openCallback(path, &name, &p);

	mask_load_data(dm, path, name, p);
}

void dh_save_data(DataHandler* dm, const char* name) {
	DataInfos* cdata = NULL;
    void* value = NULL;
    dmEventArgs* args = NULL;

    g_return_val_if_fail(dm != NULL, NULL);
	g_return_if_fail(STRING_IS_VALID(name));

	/* find the data by name */
    value = g_hash_table_lookup(dm->data, name);

    g_return_if_fail(value != NULL);
	
	cdata = POINTER_TO_DATAINFO(value);

	g_return_if_fail(STRING_IS_VALID(cdata->path));

	/* save data */
	dm->saveCallback(cdata->data, cdata->path);

    /* notify the event */
    if (g_slist_length(dm->dataUnloadEventHandler) > 0) {
        args = g_new(dmEventArgs, 1);

        args->name = g_strdup(cdata->name);
        args->path = g_strdup(cdata->path);
        args->from_file = cdata->from_file;
        args->data = cdata->data;

        rise_events(dm->dataSaveEventHandler, args);
    }
}

void dh_clear_data(DataHandler* dm) {
	g_return_if_fail(dm != NULL);

    g_hash_table_remove_all(dm->data);

    /* notify the event */
    if (g_slist_length(dm->dataClearEventHandler) > 0) {
        rise_events(dm->dataClearEventHandler, NULL);
    }
}

void* dh_get_data_by_name(DataHandler* dm, const char* name) {
    DataInfos* cdata = NULL;

    g_return_if_fail(dm != NULL);
	g_return_val_if_fail(STRING_IS_VALID(name), NULL);

    if (g_hash_table_contains(dm->data, name) == FALSE)
        return NULL;

    cdata = POINTER_TO_DATAINFO(g_hash_table_lookup(dm->data, name));

    return cdata->data;
}

const char* dh_get_data_name(DataHandler* dm, const void* data) {
	DataInfos* cdata = NULL;
    GHashTableIter iter;
    void* key = NULL;
    void* value = NULL;

    g_return_val_if_fail(dm != NULL, NULL);
	g_return_val_if_fail(data != NULL, NULL);

    /* find key by data */
    g_hash_table_iter_init(&iter, dm->data);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        cdata = POINTER_TO_DATAINFO(value);

        if (dm->cmpCallback(cdata->data,data) == TRUE) {
            return cdata->name;
        }
    }

	return NULL;
}

int dh_data_is_loaded(DataHandler* dm, const void* data) {
    g_return_val_if_fail(dm != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);

    if (STRING_IS_VALID(dh_get_data_name(dm, data)))
        return TRUE;

    return FALSE;
}

void dh_save_data_on_file(DataHandler* dm, const char* name, const char* path) {
	DataInfos* cdata = NULL;
    void* value = NULL;

	/* check input arguments */
    g_return_if_fail(dm != NULL);
	g_return_if_fail(STRING_IS_VALID(name));
	g_return_if_fail(STRING_IS_VALID(path));

	/* find the data by name */
    value = g_hash_table_lookup(dm->data, name);
	if (value == NULL) {
        print_error(get_name_exists_error(name));
        return;
    }
	
	cdata = POINTER_TO_DATAINFO(value);
	
	/* save data */
	dm->saveCallback(cdata->data, path);

	/* update data informations */
	cdata->path = g_strdup(path);
	cdata->from_file = TRUE;
}

int dh_data_is_loaded_from_file(DataHandler* dm, const void* data) {
	DataInfos* cdata = NULL;
    GHashTableIter iter;
    void* key = NULL;
    void* value = NULL;

    g_return_val_if_fail(dm != NULL, NULL);
	g_return_val_if_fail(data != NULL, NULL);

    /* find key by data */
    g_hash_table_iter_init(&iter, dm->data);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        cdata = POINTER_TO_DATAINFO(value);

        if (dm->cmpCallback(cdata->data, data) == TRUE) {
            return cdata->from_file;
        }
    }

	return FALSE;
}
