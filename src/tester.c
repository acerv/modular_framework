/*
 * tester.c
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
#include "messages.h"
#include "localization.h"
#include "engine.h"
#include "config.h"
#include "definitions.h"
#include "ui/test_window.h"

/****************************
 * Messages test functions
 ****************************/
void callback0(const PackageData *data, Package *output, unsigned int *sizeOutput) {
	PackageData outdata[2];
	outdata[0].field = GINT_TO_POINTER(1);
	outdata[1].field = GINT_TO_POINTER(2);
	
	g_print("Callback0 data = %s\n\r", (char*)data->field);
	
	output->name = "OUTPUT0";
	output->size = 2;
	output->data = outdata;

	*sizeOutput = 2;
}

void callback1(const PackageData *data, Package *output, unsigned int *sizeOutput) {
	PackageData outdata[2];
	outdata[0].field = GINT_TO_POINTER(1);
	outdata[1].field = GINT_TO_POINTER(2);
	
	g_print("Callback1 data = %s\n\r", (char*)data->field);

	output->name = "OUTPUT1";
	output->size = 2;
	output->data = outdata;
	
	*sizeOutput = 2;
}

void test_messages(void) {
	Package pkg[2];
	Package output;
	const char* const* dict = NULL;
    unsigned int dictLength = 0;
	unsigned int outSize = 0;
	size_t i = 0, j = 0;
    MessageHandler* mh = NULL;
	
	/* initialize the module */
	mh = mh_new();

	/* add methods in the message handler */
	g_print("\n\rAdd input words..\n\r");

	mh_add_word(mh, "MOVE0", callback0);
	mh_add_word(mh, "MOVE1", callback1);

	/* print dictionary */
	g_print("Dictionary is the following one\n\r");

	dict = mh_get_dictionary(mh, &dictLength);

    g_assert(dictLength == 2);

	for (i = 0; i < dictLength; i++) {
		g_print("- key: %s\n\r", dict[i]);
	}

	/* test the methods in the message handler */
	pkg[0].name = "MOVE0";
	pkg[0].size = 1;
	pkg[0].data = g_new0(PackageData, 1);
	pkg[0].data->field = "Test0";

	pkg[1].name = "MOVE1";
	pkg[1].size = 1;
	pkg[1].data = g_new0(PackageData, 1);
	pkg[1].data->field = "Test1";

	g_print("Send packets..\n\n\r");

	for (i = 0; i < 2; i++) {
		mh_send_data(mh, pkg[i], &output, &outSize);

		g_print("Output data: name = %s, size = %i" 
		         ", output size = %i", 
		         output.name, output.size, outSize); 

		for (j = 0; j < outSize; j++) {
			g_print(", output[%zu] = %i", j, GPOINTER_TO_INT(output.data[j].field));
		}

		g_print("\n\n\r");
	}

    mh_free(mh);
}

/*******************************
 * Data test functions
 *******************************/ 
void open_file_callback(const char* path, char** name, void** data) {
	char* pData = NULL;
	char* pName = NULL;
	
	g_print("Entering open_file_callback...\n\r");

	pData = "10";
	pName = "10";

	*name = pName;
	*data = pData;
}

void save_file_callback(gconstpointer data, const char* path) {
	g_print("Entering save_file_callback...\n\r");
}

int cmp_data_callback(gconstpointer data0, gconstpointer data1) {
	char* strData0 = (char*)data0;
	char* strData1 = (char*)data1;

	g_print("Entering cmp_data_callback...\n\r");

	if (g_strcmp0(strData0, strData1) == 0) {
		return TRUE;
	}

	return FALSE;
}

void load_event_callback(dmEventArgs* args) {
    g_print("Load event callback has been called (data='%s', path='%s', from_file='%i')...\n\r",
        args->name,
        args->path,
        args->from_file);
}

void unload_event_callback(dmEventArgs* args) {
    g_print("Unload event callback has been called (data='%s', path='%s', from_file='%i')...\n\r",
        args->name,
        args->path,
        args->from_file);
}

void save_event_callback(dmEventArgs* args) {
    g_print("Save event callback has been called (data='%s', path='%s', from_file='%i')...\n\r",
        args->name,
        args->path,
        args->from_file);
}

void clear_event_callback(void) {
    g_print("Clear event callback has been called...\n\r");
}

void test_data(void) {
	const unsigned int NUM_OF_DATA = 10;
	size_t i = 0;
	void* data = NULL;
	unsigned int dataLength = 0;
	char* strInt = NULL;
	int loaded = FALSE;
	const char* const* names = NULL;
	unsigned int namesLength = 0;
    DataHandler* dm = NULL;
	
	g_print("\n\rCreate data manager...\n\r");
    dm = dh_new(open_file_callback, save_file_callback, cmp_data_callback);

    g_print("Adding event handlers...\n\r");
    dh_add_load_event(dm, load_event_callback);
    dh_add_unload_event(dm, unload_event_callback);
    dh_add_save_event(dm, save_event_callback);
    dh_add_clear_event(dm, clear_event_callback);

	g_print("Start adding random data...\n\r");
	for (i = 0; i < NUM_OF_DATA; i++) {
		strInt = g_strdup_printf("%zu", i);
		
		dh_load_data(dm, strInt, strInt);
	}

    g_print("Force to load data out of bounds...\n\r");
    dh_set_max_data(dm, 0);
    dh_set_max_data(dm, G_MAXUINT);
    g_print("Max data is fixed to '%i'\n\r", dh_get_max_data(dm));
	
    dataLength = dh_get_data_size(dm);
	g_assert(dataLength == NUM_OF_DATA);
	
	g_print("The number of data loaded is %i...\n\r", dataLength);

	g_print("Check if data is loaded...\n\r");
	for (i = 0; i < NUM_OF_DATA; i++) {
		strInt = g_strdup_printf("%zu", i);
		
		loaded = dh_data_is_loaded(dm, strInt);
		g_assert(loaded == TRUE);

		if (loaded == TRUE) {
			g_print("'%s' is loaded\n\r", strInt);
		}
	}

	g_print("Simulate file saving...\n\r");
	for (i = 0; i < NUM_OF_DATA; i++) {
		strInt = g_strdup_printf("%zu", i);
		
		dh_save_data_on_file(dm, strInt, "test_path");

		loaded = dh_data_is_loaded_from_file(dm, strInt);
		g_assert(loaded == TRUE);

		if (loaded == TRUE) {
			g_print("'%s' is correctly saved on file\n\r", strInt);
		}
	}

	g_print("Simulate auto-save...\n\r");
	for (i = 0; i < NUM_OF_DATA; i++) {
		strInt = g_strdup_printf("%zu", i);
		
		dh_save_data(dm, strInt);
	}

	g_print("Simulate file opening...\n\r");

	dh_open_data(dm, "test_path");
	
	dataLength = dh_get_data_size(dm);
	g_assert(dataLength == NUM_OF_DATA + 1);

	g_print("The number of data loaded is %i\n\r", dataLength);

	names = dh_get_data_names(dm, &namesLength);

	g_assert(dataLength == namesLength);

	g_print("This is the list of the loaded items...\n\r");
	for (i = 0; i < namesLength; i++) {
		g_print("Item: %s\n\r", names[i]);
	}

	g_print("Now get 7 data...\n\r");
	data = dh_get_data_by_name(dm, "7");
	g_assert(data != NULL);

	g_print("The data of the 7 key is '%s'\n\r", (char*)data);

	g_print("Unload the 7 data...\n\r");
	dh_unload_data(dm,"7");

	dataLength = dh_get_data_size(dm);
	g_assert(dataLength == NUM_OF_DATA);

	names = dh_get_data_names(dm, &namesLength);

	g_assert(dataLength == namesLength);

	g_print("This is the list of the loaded items...\n\r");
	for (i = 0; i < namesLength; i++) {
		g_print("Item: %s\n\r", names[i]);
	}

	g_print("Clear all items...\n\r");
	dh_clear_data(dm);

	dataLength = dh_get_data_size(dm);
	g_assert(dataLength == 0);

	g_print("The number of data loaded is %i\n\r", dataLength);

    g_print("Free up data manager resources...");
    dh_free(dm);
}


/*******************************
 * Localization test functions
 *******************************/ 
void test_print_localization_strings(LocalizationHandler* lh, const char* language) {
    const char* key = NULL;
    const char* string = NULL;

    if (language == NULL) {
        key = "Save";
        string = lh_get_string(lh, key);
        g_print("%s = %s\n\r", key, string);
        key = "Open";
        string = lh_get_string(lh, key);
        g_print("%s = %s\n\r", key, string);
        key = "Close";
        string = lh_get_string(lh, key);
        g_print("%s = %s\n\r", key, string);
        key = "Exit";
        string = lh_get_string(lh, key);
        g_print("%s = %s\n\r", key, string);
    } else if (lh_language_is_supported(lh, language) == TRUE) {
        key = "Save";
        string = lh_get_localized_string(lh, key, language);
        g_print("%s[%s] = %s\n\r", key, language, string);
        key = "Open";
        string = lh_get_localized_string(lh, key, language);
        g_print("%s[%s] = %s\n\r", key, language, string);
        key = "Close";
        string = lh_get_localized_string(lh, key, language);
        g_print("%s[%s] = %s\n\r", key, language, string);
        key = "Exit";
        string = lh_get_localized_string(lh, key, language);
        g_print("%s[%s] = %s\n\r", key, language, string);
    }
}
void test_localization(void) {
    const char* filePath = NULL;
    const char* language = NULL;
    const char* testLanguage = NULL;
    const char* const* languages;
    size_t i = 0;
    LocalizationHandler* lh = NULL;

    filePath = "localization.txt";
    language = "en";
    testLanguage = "it";

    g_print("\n\rInitialize localization with '%s' file and '%s' language...\n\r", filePath, language);
    lh = lh_new(filePath, language);

    g_print("File path is '%s' ", lh_get_file_path(lh));
    g_print(", language is '%s'\n\r", lh_get_language(lh));

    languages = lh_get_supported_languages(lh);
    g_return_if_fail(languages != NULL);

    g_print("Supported languages: ");
    for (i = 0; languages[i]; i++) {
        g_print("%s ", languages[i]);
    }
    g_print("\n\r");

    g_print("Check file strings...\n\r");
    g_print("Check localized file strings...\n\r");

    test_print_localization_strings(lh, NULL);
    
    if (lh_language_is_supported(lh, testLanguage) == FALSE) {
        g_print("'%s' language is not supported...\n\r", testLanguage);
    }

    if (lh_language_is_supported(lh, language) == TRUE) {
        test_print_localization_strings(lh, language);
    }

    if (lh_language_is_supported(lh, testLanguage) == TRUE) {
        test_print_localization_strings(lh, language);
    }

    g_print("Release localization resources...\n\r");
    lh_free(lh);
}

/*******************************
 * Engine test functions
 *******************************/ 
void test_engine(void) {
    GraphicControls* controls = NULL;
    const char* modDir = NULL;
    const char* const* names = NULL;
    unsigned int namesNum = 0;
    size_t i = 0;
    Engine* engine = NULL;
    const char* configPath = NULL;
    const char* setupPath = NULL;
    const char* modulesPath = NULL;
    const char* config = "test.cfg";
    const char* setup = "test.cfg";

    modDir = ".";

    /* initialize an empty graphic controls structure */
    controls = g_new0(GraphicControls, 1);
    controls->mainControl = NULL;
    controls->configControl = NULL;
    controls->setupControl = NULL;
    controls->testControl = NULL;

    /* initialization without debug and demo flags */
    g_print("Initialize the engine...\n\r");
    engine = engine_new(controls, NORMAL_MODE, "config", "setup", modDir);
    
    g_print("The number of loaded modules is %i\n\r", engine_get_modules_num(engine));
    names = engine_get_modules_names(engine, &namesNum);

    for (i = 0; i < namesNum; i++) {
        g_print("Loaded module: '%s'\n\r", names[i]);
    }

    /* check mode */
    engine_set_config_path(engine, config);
    engine_set_setup_path(engine, setup);
    engine_set_modules_dir(engine, modDir);

    configPath = engine_get_config_path(engine);
    setupPath = engine_get_setup_path(engine);
    modulesPath = engine_get_modules_dir(engine);

    g_print("The paths are the following ones:\n\r");
    g_print("config = %s, setup = %s, modules = %s\n\r", configPath, setupPath, modulesPath);

    /* close the engine */
    g_print("Release engine resources..\n\r");
    engine_free(engine);
}

/***************************
 * Config test functions
 ***************************/
void test_config_print_cfg(CfgFile* cfg) {
    const char* const* keys = NULL;
    unsigned int keysNum = 0;
    size_t i = 0;
    const char* key = NULL;
    const char* value = NULL;

    g_assert(cfg != NULL);

    g_print("The configuration is the following one:\n\r");

    for (i = 0; i < keysNum; i++) {
        key = keys[i];
        value = cfg_get_item_value(cfg, key);
        g_print("- Key=%s,Value=%s\n\r", key, value);
    }
}

void test_config(void) {
    CfgFile* cfg = NULL;
    const char* file = NULL;
    const char* cfgname = NULL;

    file = "test.cfg";

    cfgname = "Test1";
    g_print("\n\rLoading '%s' configuration...\n\r", cfgname);
    cfg = cfg_load(file, cfgname);
    test_config_print_cfg(cfg);

    cfgname = "Test2";
    g_print("Loading '%s' configuration...\n\r", cfgname);
    cfg = cfg_load(file, cfgname);
    test_config_print_cfg(cfg);

    cfgname = "Test3";
    g_print("Creating a test configuration...\n\r");
    cfg = cfg_new(file, cfgname);

    cfg_add_item(cfg, "KeyT3_1", "Value1");
    cfg_add_item(cfg, "KeyT3_2", "Value2");
    cfg_add_item(cfg, "KeyT3_3", "Value3");

    g_print("Add the '%s' configuration\n\r", cfgname);
    cfg_store(cfg);

    g_print("The configuration size is %u...\n\r", cfg_get_size(cfg));
    g_print("Free up configuration resources...\n\r");

    cfg_free(cfg);
}

/* The main test function */
int main(int argc, char** argv) {

	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/Messages", test_messages);
	g_test_add_func ("/Data", test_data);
    g_test_add_func ("/Localization", test_localization);
    g_test_add_func ("/Engine", test_engine);
    g_test_add_func ("/Config", test_config);
	
	return g_test_run();
}
