/*
 * errors.h
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

#ifndef ERRORS_H
#define ERRORS_H

typedef enum {
	NO_ERROR = 0,

	/* data manager errors */
	DATA_ERROR_NAME_NOT_FOUND,
	DATA_ERROR_NAME_EXISTS,
	DATA_ERROR_MAX_DATA_EXCEEDED,

    /* localization errors */
    LOCALE_ERROR_LANGUAGE_NOT_SUPPORTED,

    /* engine errors */
    ENGINE_ERROR_DYN_LOADING_NOT_SUPPORTED,
    ENGINE_ERROR_LOADING_MODULE,
    ENGINE_ERROR_SYMBOL_NOT_DEFINED
} errCode;	/* error code type */

#endif
