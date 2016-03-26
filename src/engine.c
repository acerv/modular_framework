/*
 * engine.c
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
#include <gmodule.h>
#include "engine.h"
#include "errors.h"
#include "module.h"
#include "definitions.h"

#define GMODULE_TO_GPOINTER( x ) ( (gpointer) x )
#define GPOINTER_TO_GMODULE( x ) ( (GModule*) x )

/* Module routine names */
static const char* MOD_GET_NAME = "mod_get_name";
static const char* MOD_GET_VERSION = "mod_get_version";
static const char* MOD_SETUP = "mod_setup";
static const char* LOGIC_RUN = "logic_run";
static const char* LOGIC_CLOSE = "logic_close";
static const char* GRAPHIC_RUN = "graphic_run";
static const char* GRAPHIC_CLOSE = "graphic_close";
static const char* CONF_SAVE_CONFIG = "conf_save_config";
static const char* CONF_LOAD_CONFIG = "conf_load_config";
static const char* CONF_SAVE_SETUP = "conf_save_setup";
static const char* CONF_LOAD_SETUP = "conf_load_setup";
static const char* CONF_SETUP_FORM_CLOSING = "conf_setup_form_closing";
static const char* CONF_CONFIG_FORM_CLOSING = "conf_config_form_closing";

/* Engine data */
struct _Engine {
    int initialized;
    int mode;

    char* configPath;
    char* setupPath;
    char* modulesDir;

    GSList* modules;
    GSList* libraries;
};

/* Error messages */
static const char* _moduleErrorMsg = "%s:%s\n\r%s\n\r";
static const char* _moduleErrorDynLoadingMsg = "Dynamic loading is not supported on this platform.\n\r";
static const char* _moduleErrorSymbolNotDefined = "%s:%s symbol is not defined.\n\r";

static GError* get_module_loading_error(const char* filename, const char* callbackname) {
    GError *error = NULL;

    g_assert(filename != NULL);
    g_assert(callbackname != NULL);

    error = g_error_new(
        g_quark_from_string(_moduleErrorMsg),
        ENGINE_ERROR_LOADING_MODULE,
        _moduleErrorMsg,
        filename, 
        callbackname,
        g_module_error());

    return error;    
}

static GError* get_symbol_not_defined_error(const char* filename, const char* symbol) {
    GError* error = NULL;

    g_assert(filename != NULL);
    g_assert(symbol != NULL);

    error = g_error_new(
        g_quark_from_string(_moduleErrorSymbolNotDefined),
        ENGINE_ERROR_SYMBOL_NOT_DEFINED,
        _moduleErrorSymbolNotDefined,
        filename,
        symbol);

    return error;
}

static void module_init(Engine* engine, Module* module, GraphicControls* controls) {
    /* setup the module by passing graphic controls and the current engine mode */
    module->setup(controls, engine->mode);

    /* load configuration */
    module->conf_load_config(engine->configPath);

    /* load setup */
    module->conf_load_setup(engine->setupPath);

    /* run the module */
    module->logic_run();

    /* run the graphics */
    module->graphic_run();
}

static void module_close(Module* module) {
    /* close graphic */
    module->graphic_close();
             
    /* close logic */
    module->logic_close();
}

static void lib_close(GModule* module) {
    if (!g_module_close(module)) {
        g_warning("%s", g_module_error());
    }
}

static void close_all_modules(Engine* engine) {
    Module* module = NULL;
    GSList* item = NULL;

    g_return_if_fail(engine != NULL);
    g_assert(engine->initialized == TRUE);

    /* start the closing procedure */
    item = engine->modules;

    while (item) {
        module = GPOINTER_TO_MODULE(item->data);
        module_close(module);
        item = g_slist_next(item);
    }
}

static void close_all_libraries(Engine* engine) {
    GModule* lib = NULL;
    GSList* item = NULL;

    g_return_if_fail(engine != NULL);
    g_assert(engine->initialized == TRUE);

    /* close open modules references */
    item = engine->libraries;

    while (item) {
        lib = GPOINTER_TO_GMODULE(item->data);
        lib_close(lib);
        item = g_slist_next(item);
    }
}

static GError* load_modules(Engine* engine) {
    GDir* modDir = NULL;
    GModule* lib = NULL;
    Module* module = NULL;
    const char* file = NULL;
    const char* filepath = NULL;
    int success = FALSE;
    ModGetName mod_get_name = NULL;
    ModGetVersion mod_get_version = NULL;
    ModSetup mod_setup = NULL;
    LogicRun logic_run = NULL;
    LogicClose logic_close = NULL;
    GraphicRun graphic_run = NULL;
    GraphicClose graphic_close = NULL;
    ConfSaveConfig conf_save_config = NULL;
    ConfLoadConfig conf_load_config = NULL;
    ConfSaveSetup conf_save_setup = NULL;
    ConfLoadSetup conf_load_setup = NULL;
    ConfConfigFormClosing conf_config_form_closing = NULL;
    ConfSetupFormClosing conf_setup_form_closing = NULL;

    /* reset the current list of modules */
    if (engine->modules != NULL) {
        close_all_modules(engine);

        g_slist_free(engine->modules);
        engine->modules = NULL;
    }

    if (engine->libraries != NULL) {
        close_all_libraries(engine);

        g_slist_free(engine->libraries);
        engine->libraries = NULL;
    }

    /* open directory */
    modDir = g_dir_open(engine->modulesDir, 0, NULL);

    /* get files inside the directory */
    while ((file = g_dir_read_name(modDir)) != NULL) {

        /* create the complete file path */
        filepath = g_module_build_path(engine->modulesDir, file);

        /* look if the file is a module */
        lib = g_module_open(filepath, G_MODULE_BIND_LAZY);
        if (lib == NULL) {
            g_message("%s", g_module_error());
            continue;
        }

        /* module routines */
        success = g_module_symbol(lib, MOD_GET_NAME, (void**)&mod_get_name);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, MOD_GET_NAME);
        }

        if (mod_get_name == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, MOD_GET_NAME);
        }

        success = g_module_symbol(lib, MOD_GET_VERSION, (void**)&mod_get_version);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, MOD_GET_VERSION);
        }

        if (mod_get_version == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, MOD_GET_VERSION);
        }

        success = g_module_symbol(lib, MOD_SETUP, (void**)&mod_setup);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, MOD_SETUP);
        }

        if (mod_setup == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, MOD_SETUP);
        }

        /* module logic routines */
        success = g_module_symbol(lib, LOGIC_RUN, (void**)&logic_run);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, LOGIC_RUN);
        }
        
        if (logic_run == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, LOGIC_RUN);
        }

        success = g_module_symbol(lib, LOGIC_CLOSE, (void**)&logic_close);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, LOGIC_CLOSE);
        }

        if (logic_close == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, LOGIC_CLOSE);
        }

        /* module graphic routines */
        success = g_module_symbol(lib, GRAPHIC_RUN, (void**)&graphic_run);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, GRAPHIC_RUN);
        }
        
        if (graphic_run == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, GRAPHIC_RUN);
        }

        success = g_module_symbol(lib, GRAPHIC_CLOSE, (void**)&graphic_close);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, GRAPHIC_CLOSE);
        }

        if (graphic_close == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, GRAPHIC_CLOSE);
        }

        /* module conf routines */
        success = g_module_symbol(lib, CONF_SAVE_CONFIG, (void**)&conf_save_config);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, CONF_SAVE_CONFIG);
        }

        if (conf_save_config == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, CONF_SAVE_CONFIG);
        }

        success = g_module_symbol(lib, CONF_LOAD_CONFIG, (void**)&conf_load_config);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, CONF_LOAD_CONFIG);
        }

        if (conf_load_config == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, CONF_LOAD_CONFIG);
        }

        success = g_module_symbol(lib, CONF_SAVE_SETUP, (void**)&conf_save_setup);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, CONF_SAVE_SETUP);
        }

        if (conf_save_setup == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, CONF_SAVE_SETUP);
        }
        
        success = g_module_symbol(lib, CONF_LOAD_SETUP, (void**)&conf_load_setup);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, CONF_LOAD_SETUP);
        }

        if (conf_load_setup == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, CONF_LOAD_SETUP);
        }

        success = g_module_symbol(lib, CONF_CONFIG_FORM_CLOSING, (void**)&conf_config_form_closing);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, CONF_CONFIG_FORM_CLOSING);
        }
 
        if (conf_config_form_closing == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, CONF_CONFIG_FORM_CLOSING);
        }

        success = g_module_symbol(lib, CONF_SETUP_FORM_CLOSING, (void**)&conf_setup_form_closing);
        if (success == FALSE) {
            lib_close(lib);
            return get_module_loading_error(file, CONF_SETUP_FORM_CLOSING);
        }

        if (conf_setup_form_closing == NULL) {
            lib_close(lib);
            return get_symbol_not_defined_error(file, CONF_SETUP_FORM_CLOSING);
        }

        /* create the module instance */
        module = g_new0(Module, 1);

        module->get_name = mod_get_name;
        module->get_version = mod_get_version;
        module->setup = mod_setup;
        module->logic_run = logic_run;
        module->logic_close = logic_close;
        module->graphic_run = graphic_run;
        module->graphic_close = graphic_close;
        module->conf_save_config = conf_save_config;
        module->conf_load_config = conf_load_config;
        module->conf_save_setup = conf_save_setup;
        module->conf_load_setup = conf_load_setup;
        module->conf_config_form_closing = conf_config_form_closing;
        module->conf_setup_form_closing = conf_setup_form_closing;

        engine->modules = g_slist_append(engine->modules, MODULE_TO_GPOINTER(module));

        /* save the GModule reference file */
        engine->libraries = g_slist_append(engine->libraries, GMODULE_TO_GPOINTER(lib));
    }

    return NULL;
}

/* Implementations */
Engine* engine_new(GraphicControls* controls, int mode, 
    const char* configPath, const char* setupPath, const char* modulesDir) {
    GError* error = NULL;
    Module* module = NULL;
    GSList* item = NULL;
    Engine* engine = NULL;

    g_return_val_if_fail(controls != NULL, NULL);
    g_return_val_if_fail(STRING_IS_VALID(configPath), NULL);
    g_return_val_if_fail(STRING_IS_VALID(setupPath), NULL);
    g_return_val_if_fail(STRING_IS_VALID(modulesDir), NULL);

    /* check if dynamic loading is supported on this platform */
    if (g_module_supported() == FALSE) {
        error = g_error_new(
            g_quark_from_string(_moduleErrorDynLoadingMsg),
            ENGINE_ERROR_DYN_LOADING_NOT_SUPPORTED,
            _moduleErrorDynLoadingMsg, 
            NULL);

        print_error(error);
        return NULL;
    }

    engine = g_new(Engine, 1);
    engine->initialized = FALSE;
    engine->configPath = g_strdup(configPath);
    engine->setupPath = g_strdup(setupPath);
    engine->modulesDir = g_strdup(modulesDir);
    engine->modules = NULL;
    engine->libraries = NULL;

    /* save engine modes */
    engine->mode = mode;

    /* load modules */
    error = load_modules(engine);
    if (error) { 
        print_error(error);

        /* close open libraries and modules */
        close_all_modules(engine);
        close_all_libraries(engine);

        return NULL; 
    }

    /* initialize modules */
    item = engine->modules;

    while (item) {
        module = GPOINTER_TO_MODULE(item->data);
        module_init(engine, module, controls);
        item = g_slist_next(item);
    }

    /* the engine has been initialized */
    engine->initialized = TRUE;

    return engine;
}

void engine_free(Engine* engine) {
    g_return_if_fail(engine != NULL);
    g_assert(engine->initialized == TRUE);

    close_all_modules(engine);
    close_all_libraries(engine);

    g_slist_free(engine->modules);
    g_slist_free(engine->libraries);
    g_free(engine->configPath);
    g_free(engine->setupPath);
    g_free(engine->modulesDir);
    g_free(engine);
}

int engine_is_demo(Engine* engine) {
    g_return_val_if_fail(engine != NULL, FALSE);
    g_assert(engine->initialized == TRUE);

    return engine->mode == DEMO_MODE;
}

int engine_is_debug(Engine* engine) {
    g_return_val_if_fail(engine != NULL, FALSE);
    g_assert(engine->initialized == TRUE);
    
    return engine->mode == DEBUG_MODE;
}

void engine_set_config_path(Engine* engine, const char* path) {
    g_return_if_fail(engine != NULL);
    g_assert(engine->initialized == TRUE);
    g_return_if_fail(g_file_test(path, G_FILE_TEST_EXISTS) == TRUE);
    g_return_if_fail(g_file_test(path, G_FILE_TEST_IS_REGULAR) == TRUE);

    engine->configPath = g_strdup(path);
}

void engine_set_setup_path(Engine* engine, const char* path) {
    g_return_if_fail(engine != NULL);
    g_assert(engine->initialized == TRUE);
    g_return_if_fail(g_file_test(path, G_FILE_TEST_EXISTS) == TRUE);
    g_return_if_fail(g_file_test(path, G_FILE_TEST_IS_REGULAR) == TRUE);

    engine->setupPath = g_strdup(path);
}

void engine_set_modules_dir(Engine* engine, const char* directory) {
    g_return_if_fail(engine != NULL);
    g_assert(engine->initialized == TRUE);
    g_return_if_fail(g_file_test(directory, G_FILE_TEST_EXISTS) == TRUE);
    g_return_if_fail(g_file_test(directory, G_FILE_TEST_IS_DIR) == TRUE);

    engine->modulesDir = g_strdup(directory);
}

const char* engine_get_config_path(Engine* engine) {
    g_return_val_if_fail(engine != NULL, NULL);
    g_assert(engine->initialized == TRUE);

    return engine->configPath;
}

const char* engine_get_setup_path(Engine* engine) {
    g_return_val_if_fail(engine != NULL, NULL);
    g_assert(engine->initialized == TRUE);

    return engine->setupPath;
}

const char* engine_get_modules_dir(Engine* engine) {
    g_return_val_if_fail(engine != NULL, NULL);
    g_assert(engine->initialized == TRUE);

    return engine->modulesDir;
}

unsigned int engine_get_modules_num(Engine* engine) {
    g_return_val_if_fail(engine != NULL, NULL);
    g_assert(engine->initialized == TRUE);

    if (engine->modules == NULL)
        return 0;

    return g_slist_length(engine->modules);
}

const char* const* engine_get_modules_names(Engine* engine, unsigned int* size) {
    const char** names = NULL;
    const char* name = NULL;
    unsigned int length = 0;
    size_t i = 0;
    GSList* item = NULL;
    Module* module = NULL;

    g_return_val_if_fail(engine != NULL, NULL);
    g_assert(engine->initialized == TRUE);

    length = engine_get_modules_num(engine);

    if (length < 1)
        return NULL;

    /* create names array */
    names = g_malloc(length * sizeof(char*));

    *size = length;

    /* fetch modules' names */
    item = engine->modules;

    while (item) {
        /* get module's name */
        module = GPOINTER_TO_MODULE(item->data);
        name = module->get_name();

        g_assert(STRING_IS_VALID(name));

        /* save the name */
        names[i] = g_strdup(name);
        
        item = g_slist_next(item);

        i++;
    }

    return names;
}
