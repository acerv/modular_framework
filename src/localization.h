/*
 * localization.h
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

#ifndef LOCALIZATION_H
#define LOCALIZATION_H

/* Abstract data type that rapresents the localization handler */
struct _LocalizationHandler;
typedef struct _LocalizationHandler LocalizationHandler;

/* Creates a localization handler */
LocalizationHandler* lh_new(const char* path, const char* language);

/* Releases a localization handler resources */
void lh_free(LocalizationHandler* lh);

/* Loads the localization file  */
void lh_load_file(LocalizationHandler* lh, const char* path);

/* Returns the localization file path */
const char* lh_get_file_path(LocalizationHandler* lh);

/* Returns TRUE if language is supported. FALSE otherwise  */
int lh_language_is_supported(LocalizationHandler* lh, const char* language);

/* Returns the array of supported languages */
const char* const* lh_get_supported_languages(LocalizationHandler* lh);

/* Sets the default language */
void lh_set_language(LocalizationHandler* lh, const char* language);

/* Returns the current language */
const char* lh_get_language(LocalizationHandler* lh);

/* Returns a localized string */
const char* lh_get_string(LocalizationHandler* lh, const char* key);

/* Returns a string, localized for the given language */
const char* lh_get_localized_string(LocalizationHandler* lh, const char* key, const char* language);

#endif
