/*
 * test_window.c
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

#include <gtk/gtk.h>
#include "test_window.h"
#include "../definitions.h"

struct _TestWindow {
    GtkWidget* dialog; /* the dialog */
    GtkWidget* tabs;   /* the tabs container */
};

static unsigned int validate_container(const TestWindow* window) {
    g_return_val_if_fail(window != NULL, FALSE);
    g_return_val_if_fail(window->dialog != NULL, FALSE);
    g_return_val_if_fail(window->tabs != NULL, FALSE);

    return TRUE;
}

/*******************
 * IMPLEMENTATIONS 
 *******************/
TestWindow* test_window_new(void* parent) {
    TestWindow* window_container = NULL;
    GtkWindow* parentWidget = NULL;
    GtkWidget* dialog = NULL;
    GtkWidget* tabs = NULL;
    GtkBox* contentarea = NULL;
    GtkDialogFlags flags;

    g_assert(parent != NULL);

    parentWidget = GTK_WINDOW(parent);
    g_assert(parentWidget != NULL);

    /* create dialog */
    flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons(
        "Test panel",
        parentWidget,
        flags,
        "_OK",
        GTK_RESPONSE_ACCEPT,
        "Cancel",
        GTK_RESPONSE_REJECT,
        NULL); /* ending with NULL */

    contentarea = GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));

    /* create tabs */
    tabs = gtk_notebook_new(); 
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(tabs), GTK_POS_LEFT);

    /* add tabs */
    gtk_box_pack_start(contentarea, tabs, 1, 1, 1);

    /* show tabs */
    gtk_widget_show(tabs);

    /* create the container */
    window_container = g_new(TestWindow, 1);
    window_container->dialog = dialog;
    window_container->tabs = tabs;

    return window_container;
}

void test_window_destroy(TestWindow* window) {
    g_assert(window != NULL);

    gtk_widget_destroy(window->dialog);

    g_free(window);
}

void test_window_add_tab(TestWindow* window, void* tabcontrol, const char* name) {
    GtkWidget* tab = NULL;
    GtkWidget* label = NULL;

    g_return_if_fail(validate_container(window) == TRUE);
    g_return_if_fail(tabcontrol != NULL);
    g_return_if_fail(STRING_IS_VALID(name));

    /* get the tab */
    tab = GTK_WIDGET(tabcontrol);
    
    g_return_if_fail(tab != NULL);

    /* create the label */
    label = gtk_label_new(name);
    
    /* add the tab */
    gtk_notebook_append_page(GTK_NOTEBOOK(window->tabs), tab, label);
    
    /* show tab */
    gtk_widget_show(tab);
}

void test_window_remove_tab(TestWindow* window, const char* name) {
    size_t size = 0;
    size_t i = 0;
    GtkWidget* page = NULL; 
    GtkNotebook* tabs = NULL;
    const char* label = NULL;

    g_return_if_fail(validate_container(window) == TRUE);
    g_return_if_fail(STRING_IS_VALID(name));
    
    tabs = GTK_NOTEBOOK(window->tabs);
    size = gtk_notebook_get_n_pages(tabs);

    /* find page with the given label */
    for (i = 0; i < size; i++) {
        page = gtk_notebook_get_nth_page(tabs, i);

        g_assert(page != NULL);

        label = gtk_notebook_get_tab_label_text(tabs, page);

        /* if found, remove the tab */
        if (g_strcmp0(label, name) == 0) {
            gtk_notebook_remove_page(tabs, i);
            break;
        }
    }
}

void test_window_show(TestWindow* window) {
    g_return_if_fail(validate_container(window) == TRUE);

    /* show dialog causes UI locking */
    gtk_dialog_run(GTK_DIALOG(window->dialog));

    /* then hide the window */
    gtk_widget_hide(window->dialog);
}
