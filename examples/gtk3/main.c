/*
 * main.c
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

int main(int argc, char** argv) {
    GtkBuilder* builder;
    GtkWidget* window;

    gtk_init(&argc, &argv);

    /* fetch the file */
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "baseframe.xml", NULL);

    /* create the window */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
   
    /* add signals */
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* free builder */
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(G_OBJECT(builder));

    /* show main form */
    gtk_widget_show(window);
    gtk_main();

    return 0;
}
