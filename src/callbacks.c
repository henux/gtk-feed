
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "common.h"
#include "dialogs.h"

/* The "activate" handler of the system tray icon.  ICON is the system tray
   status icon object and USER_DATA is ignored.  This event handlers pops
   up the feeds menu and displays it to the user. */
void
on_icon_activate (GtkStatusIcon *icon,
                  gpointer       user_data)
{
  gtk_menu_popup (get_feeds_menu (),
                  NULL,
                  NULL,
                  gtk_status_icon_position_menu,
                  icon,
                  0,
                  gtk_get_current_event_time ());
}

/* The "popup-menu" handler of the system tray icon.  ICON is the system
   tray status icon object and USER_DATA is ignored.  This event handler
   pops up the application menu and displayes it to the user. */
void
on_icon_popup_menu (GtkStatusIcon *icon,
                    guint          button,
                    guint          activate_time,
                    gpointer       user_data)
{
  gtk_menu_popup (get_main_menu (),
                  NULL,
                  NULL,
                  gtk_status_icon_position_menu,
                  icon,
                  button,
                  activate_time);
}

/* The "activate" handler of the feeds menu item.  ITEM is the menu item
   object and USER_DATA points to a null-terminated string specifying the
   URL of the feed article.  This event handler opens feed URL in a web
   browser. */
void
on_feed_open (GtkMenuItem *item,
              gpointer     user_data)
{
  open_url ((const gchar *) user_data, NULL);
}

/* The "About" main menu item handler.  ITEM is the menu item object and
   USER_DATA is ignored.  This event handler shows the about dialog to the
   user. */
void
on_main_about (GtkMenuItem *item,
               gpointer     user_data)
{
  show_about_dialog ();
}

/* The "Subscribe" main menu item handler.  ITEM is the menu item object
   and USER_DATA is ignored.  This event handler shows the subscribe dialog
   to the user. */
void
on_main_subscribe (GtkMenuItem *item,
                   gpointer     user_data)
{
  show_subscribe_dialog ();
}

/* The "Quit" main menu item handler.  ITEM is the menu item object and
   USER_DATA is ignored.  This event handler signals exit from GTK main
   loop. */
void
on_main_quit (GtkMenuItem *item,
              gpointer     user_data)
{
  gtk_main_quit ();
}
