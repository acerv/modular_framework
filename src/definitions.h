/*
 * definitions.h
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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <glib.h>

/********************
 *		MACRO
 ********************/ 
#define STRING_TO_POINTER( x ) ( (void*) x )
#define POINTER_TO_STRING( x ) ( (char*) x )
#define STRING_IS_VALID( x ) ( x != NULL && !g_str_equal (x, "") )

/* it prints the error message and free up the error object resources */
#define print_error( error ) do { \
        g_assert(error != NULL); \
        g_warning("%s", error->message); \
        g_error_free(error); \
    } while(0)

/********************
 *      TYPEDEF
 *******************/
typedef void (*voidEventFunc)(void);

#endif
