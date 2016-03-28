
# compiler
CC=gcc
CC_STANDARD=-std=c99

# directories
BUILD_DIR=build
SRC_DIR=src
TEST_DIR=test_files
MK_BUILD_DIR=mkdir -p $(BUILD_DIR)

# debug flags
DEBUG_ENABLE=1
DEBUG_CFLAGS=-g -DDEBUG

# dependencies
CFLAGS_DEPEND=`pkg-config --cflags --libs glib-2.0` `pkg-config --cflags --libs gmodule-2.0`

# default flags
ifeq ($(DEBUG_ENABLE), 1)
CFLAGS=$(DEBUG_CFLAGS) $(CC_STANDARD) -pedantic -Wall -fPIC $(CFLAGS_DEPEND) 
TEST_MODULE_CFLAGS=$(DEBUG_CFLAGS) $(CC_STANDARD) -pedantic -Wall -fPIC -shared $(CFLAGS_DEPEND)
else
CFLAGS=$(CC_STANDARD) -pedantic -Wall -fPIC $(CFLAGS_DEPEND)
TEST_MODULE_CFLAGS=$(CC_STANDARD) -pedantic -Wall -fPIC -shared $(CFLAGS_DEPEND)
endif

# test options
TEST_SOURCES=$(addprefix $(SRC_DIR)/,utils.c messages.c data.c localization.c engine.c config.c tester.c)
TEST_OBJECTS=$(addprefix $(SRC_DIR)/,utils.o messages.o data.o localization.o engine.o config.o tester.o)
TEST_MODULE_SRC=$(addprefix $(SRC_DIR)/,test_module.c)
TEST_MODULE_LIB=$(addprefix $(SRC_DIR)/,libtestmodule.so)
BUILT_TEST_MODULE_LIB=$(addprefix $(TEST_DIR)/,libtestmodule.so)
TEST_EXECUTABLE=$(addprefix $(BUILD_DIR)/,tester)
TEST_FILES=$(addprefix $(TEST_DIR)/,*)

.PHONY: all test

all: test

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	$(MK_BUILD_DIR)

$(TEST_MODULE_LIB): $(TEST_MODULE_SRC)
	$(CC) $(TEST_MODULE_CFLAGS) $(TEST_MODULE_SRC) -o $@

$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(TEST_MODULE_LIB)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) -o $@

test: $(BUILD_DIR) $(TEST_EXECUTABLE)
	rsync --remove-source-files $(TEST_MODULE_LIB) $(TEST_DIR)/ && \
	cp -rf $(TEST_FILES) $(BUILD_DIR)

clean:
	rm -rf $(TEST_OBJECTS) $(BUILT_TEST_MODULE_LIB) $(LIBS_OBJECT) $(ALL_OBJECTS) $(BUILD_DIR)
