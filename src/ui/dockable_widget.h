/*
 * dockable_form.h
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

#ifndef DOCKABLE_FORM_H
#define DOCKABLE_FORM_H

/* the dockable form */
struct _DockableForm;
typedef struct _DockableForm DockableForm;

/* create dockable form */
DockableForm* dockable_form_new(void);

/* destroy dockable form */
void dockable_form_destroy(DockableForm* form);

#endif
