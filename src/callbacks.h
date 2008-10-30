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

#ifndef CALLBACKS_H
#define CALLBACKS_H

/* System tray icon callbacks */
void on_icon_activate (GtkStatusIcon *, gpointer);
void on_icon_popup_menu (GtkStatusIcon *, guint, guint, gpointer);

/* Feed menu callbacks */
void on_feed_open (GtkMenuItem *, gpointer);

/* Main menu callbacks */
void on_main_subscribe (GtkMenuItem *, gpointer);
void on_main_feeds (GtkMenuItem *, gpointer);
void on_main_about (GtkMenuItem *, gpointer);
void on_main_quit (GtkMenuItem *, gpointer);

#endif
