/*
 * messages.c
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
#include "messages.h"
#include "definitions.h"
#include "utils.h"

/* The message handler */
struct MessageHandler_type {
    GHashTable* dictionary;
};

typedef struct {
	msgCallback callback;
} callbackContainer;

static callbackContainer* new_callbackContainer(msgCallback callback) {
	callbackContainer* cont;

	cont = g_new(callbackContainer, 1);
	cont->callback = callback;

	return cont;
}

static void add_item_to_dictionary(GHashTable* dict, const char* word, msgCallback callback) {
	void* wordPtr = NULL;
	callbackContainer* cont = NULL;
	
	g_return_if_fail(dict != NULL);
	g_return_if_fail(word != NULL);
	g_return_if_fail(callback != NULL);

	wordPtr = (void**)g_strdup(word);
	cont = new_callbackContainer(callback);
	
	g_hash_table_insert(dict, wordPtr, cont);
}

/* Implementations */
MessageHandler* mh_new(void) {
    MessageHandler* mh = NULL;

    mh = g_new(MessageHandler, 1);
    mh->dictionary = g_hash_table_new(g_str_hash, g_str_equal);

    return mh;
}

void mh_free(MessageHandler* mh) {
    g_assert(mh != NULL);

    /* remove dictionary */
    g_hash_table_remove_all(mh->dictionary);

    mh->dictionary = NULL;
    g_free(mh);
    
    mh = NULL;
}

const char* const* mh_get_dictionary(MessageHandler* mh, unsigned int* size) {
    g_return_val_if_fail(mh != NULL, NULL);

    return g_hash_table_return_keys_to_array(mh->dictionary, &size);
}

void mh_add_word(MessageHandler* mh, const char* word, msgCallback callback) {
    g_return_if_fail(mh != NULL);
    g_return_if_fail(word != NULL);
    g_return_if_fail(callback != NULL);

	add_item_to_dictionary(mh->dictionary, word, callback);
}

void mh_send_data(MessageHandler* mh, const Package pkg, Package* output, unsigned int* sizeOutput) {
	void* key = NULL;
	void* val = NULL;
	msgCallback callback = NULL;
	
    g_return_if_fail(mh != NULL);
	g_return_if_fail(output != NULL);
	g_return_if_fail(sizeOutput != NULL);
	g_return_if_fail(pkg.size <= G_MAXUINT);
	
	/* find the dictionary element */
	key = g_strdup(pkg.name);
	
	if (g_hash_table_contains(mh->dictionary, key)) {
		val = g_hash_table_lookup(mh->dictionary, key);

		g_assert(val != NULL);

		/* get the callback */
		callback = ((callbackContainer*)val)->callback;
		
		/* run the callback */
		((msgCallback)callback) (pkg.data, output, sizeOutput);
	}
}
