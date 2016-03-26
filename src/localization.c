/*
 * localization.c
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
#include "localization.h"
#include "errors.h"
#include "definitions.h"

#define DEFAULT_LANGUAGE "en"
#define DEFAULT_GROUP "locale"

/* The localization handler */
struct _LocalizationHandler  {
    char* path;
    char* language;
    GKeyFile* file;
    const char* const* supportedLanguages;
};

static char* get_supported_language(LocalizationHandler* lh, const char* language) {
    int languageFound = FALSE;
    char* supportedLanguage = NULL;

    if (g_strcmp0(language, DEFAULT_LANGUAGE))
        return DEFAULT_LANGUAGE;

    if (language != NULL) {
        languageFound = lh_language_is_supported(lh, language);
        g_return_val_if_fail(languageFound == TRUE, NULL);

        supportedLanguage = g_strdup(language);
    } else {
        supportedLanguage = g_strdup(DEFAULT_LANGUAGE);
    }
    
    return supportedLanguage;
}

/* Implementations */
LocalizationHandler* lh_new(const char* filePath, const char* language) {
    LocalizationHandler* lh = NULL;

    lh = g_new(LocalizationHandler, 1);
    lh->file = g_key_file_new();
    lh->supportedLanguages = g_get_language_names();

    g_return_if_fail(lh->supportedLanguages != NULL);
    
    lh_load_file(lh, filePath);
    lh_set_language(lh, language);

    return lh;
}

void lh_free(LocalizationHandler* lh) {
    g_return_if_fail(lh != NULL);

    g_key_file_free(lh->file);
    g_free(lh->path);
    g_free(lh->language);
    
    lh->file = NULL;
    lh->supportedLanguages = NULL;
    lh->path = NULL;
    lh->language = NULL;

    g_free(lh);
}

void lh_load_file(LocalizationHandler* lh, const char* path) {
    GKeyFileFlags flags;
    GError *error = NULL;

    g_return_if_fail(lh != NULL);
    g_return_if_fail(path != NULL);
    g_return_if_fail(g_file_test(path, G_FILE_TEST_EXISTS) == TRUE);
    g_return_if_fail(g_file_test(path, G_FILE_TEST_IS_REGULAR) == TRUE);

    /* copy the localization file string */
    lh->path = g_strdup(path);

    /* load localization file */
    flags = G_KEY_FILE_KEEP_TRANSLATIONS;

    if (!g_key_file_load_from_file(lh->file, path, flags, &error)) {
        print_error(error);
        return;
    }
}

const char* lh_get_file_path(LocalizationHandler* lh) {
    g_return_val_if_fail(lh != NULL, NULL);
    g_return_val_if_fail(lh->path != NULL, NULL);
    
    return g_strdup(lh->path);
}

const char* const* lh_get_supported_languages(LocalizationHandler* lh) {
    g_return_val_if_fail(lh != NULL, NULL);

    return lh->supportedLanguages;
}

int lh_language_is_supported(LocalizationHandler* lh, const char* language) {
    size_t i = 0;
    char* fetchLanguage = NULL;
    int languageFound = FALSE;

    g_return_val_if_fail(lh != NULL, FALSE);
    g_return_val_if_fail(language != NULL, FALSE);

    g_assert(lh->supportedLanguages != NULL);

    /* check if language is supported */
    for (i = 0; lh->supportedLanguages[i]; i++) {
        fetchLanguage = g_strdup(lh->supportedLanguages[i]);

        g_assert(fetchLanguage != NULL);

        if (g_strcmp0(fetchLanguage, language) == 0) {
            languageFound = TRUE;
            break;
        }
    }

    return languageFound;
}

void lh_set_language(LocalizationHandler* lh, const char* language) {
    GError* error = NULL;
    char* errorMsg = NULL;

    g_return_if_fail(lh != NULL);
    g_return_if_fail(language != NULL);

    /* check if language is supported */
    if (lh_language_is_supported(lh, language) == FALSE) {
         errorMsg = "The '%s' language is not supported.";
         error = g_error_new(
            g_quark_from_string(errorMsg),
            LOCALE_ERROR_LANGUAGE_NOT_SUPPORTED,
            errorMsg,
            language);

         print_error(error);
         return;
    }

    /* setup the language if supported */
    lh->language = g_strdup(language);
}

const char* lh_get_language(LocalizationHandler* lh) {
    char* language = NULL;

    g_return_val_if_fail(lh != NULL, NULL);

    language = g_strdup(lh->language);

    return language;
}

const char* lh_get_string(LocalizationHandler* lh, const char* key) {
    return lh_get_localized_string(lh, key, DEFAULT_LANGUAGE);
}

const char* lh_get_localized_string(LocalizationHandler* lh, const char* key, const char* language) {
    char* value = NULL;
    char* supportedLanguage = NULL;

    g_return_val_if_fail(lh != NULL, NULL);
    g_return_val_if_fail(key != NULL, NULL);

    supportedLanguage = get_supported_language(lh, language);

    value = g_key_file_get_locale_string(lh->file, DEFAULT_GROUP, key, supportedLanguage, NULL);

    return value;
}

