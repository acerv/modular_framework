
# Introduction

[![Build Status](https://travis-ci.org/acerv/modular_framework.svg?branch=master)](https://travis-ci.org/acerv/modular_framework)

This framework has been created to provide a generic system that can be used for desktop applications. 
The system provides some tools as the following ones:
* generic data handle
* messages handling
* configuration manager
* localization handle
* dynamic modules loader
* ui library independent handler

## Used libraries and dependences
GLib is the only requirement. The code has been tested on Debian stable, but it should work in all GNU/Linux
distributions which provide GLib.

## Code style
The code is massively using the Abstract-Data-Type encapsulation mechanism, to hide details of the framework
implementation. The type definition is using the "Example\_type" string declaration, while the abstract type uses "Example".

## Setup and Config
The framework is using two different configurations: setup and config, both with .ini format.
The setup is the user visible configuration and the config is the manufacter configuration of the application.
This system can be useful when the application needs to hide some configurations from the user, such as internal
uneditable parameters for the correct framework functioning.
Note that the engine will always initialize the system by fetching config first, then the setup.

## User interface
The framework is providing a generic code interface that can be implemented by using the desired library (ie Qt, Gtk+).
The interfice code can be found inside the "ui/" path and it's used by the modules of the framework.
The user interface has the following components:
* setup panel: the panel that contains all the setup controls fetched from the modules
* config panel: the panel that contains all the config controls fetched from the modules
* test panel: the panel that contains all the test controls fetched from the modules
* base window: the main form

## Module
A module is a part of the framework that's loaded at run-time. It contains its own setup, config, panels of the user
interface, and it's defined inside the file module.h. The module can be splitted in the following 3 parts:
  1. configuration: it contains config and setup of the module
  2. logic: it contains the logic of the module, like algorithms
  3. graphics: it contains the UI controls

Every module is loaded by the engine and it's initialized as following:
  1. setup the module by calling the "setup" routine
  2. load the configuration by calling the "conf\_load\_config" routine
  3. load the setup by calling the "conf\_load\_setup" routine
  4. initialize the module's logic by calling the "login\_run" routine
  5. initialize the module's UI by calling the "graphic\_run" routine

In the logic initialization, it's possible to load algorithms according with the configuration or setup.
In the graphics initialization, it's possible to load controls into the config panel, setup panel, test panel and base
window.

## Library usage
I'm not providing the UI implementation, but it's possible to find a super simple GTK+3 example in the "examples/" path. 
To use the modular framework, it's possible to include the sources inside the project and to start implement the module.h interface.
An example of the engine initialization can be found in the tester.c source code.

## Credits
Part of the engine has been thought with Gianfranco Gallizia (aka. skyglobe) in the 2013-2014 and, initially, it was a C# implementation. 
I ported the source code into C using GLib to make it multi-platform.

https://github.com/skyglobe

## Links

https://developer.gnome.org/glib/

https://developer.gnome.org/glib/stable/glib-Dynamic-Loading-of-Modules.html
