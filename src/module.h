/*
 * module.h
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

#ifndef MODULE_H
#define MODULE_H

#include "ui/base_window.h"
#include "ui/config_window.h"
#include "ui/test_window.h"

#define GPOINTER_TO_MODULE( x ) ( (Module*) x )
#define MODULE_TO_GPOINTER( x ) ( (gpointer*) x )

/* User Interface controls */
typedef struct {
    const BaseWindow* mainControl;
    const ConfigWindow* configControl;
    const ConfigWindow* setupControl;
    const TestWindow* testControl;
} GraphicControls;

/* Module routines */
typedef const char* (*ModGetName) (void);
typedef const char* (*ModGetVersion) (void);
typedef void (*ModSetup) (GraphicControls* controls, int mode);

/* Module logic routines */
typedef void (*LogicRun) (void);
typedef void (*LogicClose) (void);

/* Module graphic routines */
typedef void (*GraphicRun) (void);
typedef void (*GraphicClose) (void);

/* Module configuration routines */
typedef void (*ConfSaveConfig) (const char* filepath);
typedef void (*ConfLoadConfig) (const char* filepath);
typedef void (*ConfSaveSetup) (const char* filepath);
typedef void (*ConfLoadSetup) (const char* filepath);
typedef void (*ConfSetupFormClosing) (int saveRequest);
typedef void (*ConfConfigFormClosing) (int saveRequest);

/* A generic module */
typedef struct {
    ModGetName get_name;                                /* it returns the name of the module */
    ModGetVersion get_version;                          /* it returns the version of the module */
    ModSetup setup;                                     /* it setups the module */
    LogicRun logic_run;                                 /* it runs the module's logic */
    LogicClose logic_close;                             /* it closes the module's logic */
    GraphicRun graphic_run;                             /* it runs the module's graphic */
    GraphicClose graphic_close;                         /* it closes the module's graphic */
    ConfSaveConfig conf_save_config;                    /* it saves the module's configuration */
    ConfLoadConfig conf_load_config;                    /* it loads the module's configuration */
    ConfSaveSetup conf_save_setup;                      /* it saves the module's setup */
    ConfLoadSetup conf_load_setup;                      /* it loads the module's setup */
    ConfConfigFormClosing conf_config_form_closing;     /* this routine is launched when the config form is closing */
    ConfSetupFormClosing conf_setup_form_closing;       /* this routine is launched when the setup form is closing */
} Module;

#endif
