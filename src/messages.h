/*
 * messages.h
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

#ifndef MESSAGES_H
#define MESSAGES_H

/* Abstract data type that rapresents the message handler */
struct _MessageHandler;
typedef struct _MessageHandler MessageHandler;

typedef struct {
	void* field; 	/* data field */
} PackageData;

typedef struct {
	char* name;     	/* name of the parameter */
	unsigned int size;	/* size of the data */
	PackageData *data;  /* data */
} Package;				/* general package for the message handler */

/* The callback related to a message. */
typedef void (*msgCallback)(const PackageData *data, Package *output, unsigned int *sizeOutput);

/* Creates a new module. */
MessageHandler* mh_new(void);

/* Free up message handler resources */
void mh_free(MessageHandler* mh);

/* Return dictionary routines. */
const char* const* mh_get_dictionary(MessageHandler* mh, unsigned int* size);

/* Add dictionary word routines related to the callback. */
void mh_add_word(MessageHandler* mh, const char* word, msgCallback callback);

/* It sends the data and it initializes the output array with output data. */
void mh_send_data(MessageHandler* mh, const Package pkg, Package *output, unsigned int *sizeOutput);

#endif
