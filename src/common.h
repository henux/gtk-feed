/*
Copyright (C) 2008 Henri Häkkinen.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef COMMON_H
#define COMMON_H

/* Global singletons */
GtkStatusIcon * get_status_icon ();
GtkMenu *       get_feeds_menu ();
GtkMenu *       get_main_menu ();

/* Misc. helpers */
gboolean        open_url (const gchar *url, GError **error);

/* Menu helpers */
GtkMenuItem *   add_menu_item (GtkMenu *menu, const gchar *title);
GtkMenuItem *   add_menu_item_italic (GtkMenu *menu, const gchar *title);
GtkMenuItem *   add_menu_item_link (GtkMenu *menu, const gchar *title, const gchar *link);

void            set_menu_item (GtkMenuItem *item, const gchar *title);
void            set_menu_item_italic (GtkMenuItem *item, const gchar *title);

#endif
