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

/* Creates the singleton status icon object.  Subsequent calls will return
   the same status icon object. */
GtkStatusIcon *
get_status_icon ()
{
  static GtkStatusIcon *status_icon = NULL;

  if (status_icon == NULL) {
    status_icon = gtk_status_icon_new_from_icon_name ("gtk-feed");

    g_signal_connect (status_icon,
                      "activate",
                      G_CALLBACK(on_icon_activate),
                      NULL);

    g_signal_connect (status_icon,
                      "popup-menu",
                      G_CALLBACK(on_icon_popup_menu),
                      NULL);
  }

  return status_icon;
}

/* Creates the singleton feeds menu object.  Subsequent calls will return
   the same menu object. */
GtkWidget *
get_feeds_menu ()
{
  static GtkWidget *feeds_menu = NULL;

  if (feeds_menu == NULL) {
    /* Simply creates the singleton menu object for the feeds menu;
       loading of feeds is done elsewhere. */
    feeds_menu = gtk_menu_new ();
    gtk_widget_show_all (feeds_menu);
  }
  
  return feeds_menu;
}

/* Creates the singleton main menu object.  Subsequent calls will return
   the same menu object. */
GtkWidget *
get_main_menu ()
{ 
  static GtkWidget *main_menu = NULL;

  if (main_menu == NULL) {
    GtkWidget *item, *image;

    /* Menu object. */
    main_menu = gtk_menu_new ();

    /* Subscribe menu item. */
    image = g_object_new (GTK_TYPE_IMAGE,
                          "stock", GTK_STOCK_ADD,
                          "icon-size", GTK_ICON_SIZE_MENU,
                          NULL);

    item = gtk_image_menu_item_new_with_mnemonic ("_Subscribe");

    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(item),
                                   image);

    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);

    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_subscribe),
                      NULL);

    /* Feeds menu item. */
    image = gtk_image_new_from_icon_name ("gtk-feed",
                                          GTK_ICON_SIZE_MENU);

    item = gtk_image_menu_item_new_with_mnemonic ("_Feeds");

    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(item),
                                   image);

    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);

    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_feeds),
                      NULL);

    /* About menu item. */
    item = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT,
                                               NULL);

    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);

    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_about),
                      NULL);

    /* Quit menu item. */
    item = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT,
                                               NULL);

    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);

    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_quit),
                      NULL);

    /* Show the menu. */
    gtk_widget_show_all (main_menu);
  }

  return main_menu;
}

/* Open the given URL in the system's web browser.  This is done by running
   the command `xdg-open URL'.  Returns TRUE if the operation succeeded, or
   FALSE if error occured.  The caller may opt to pass a pointer in the
   ERROR parameter to get more details. */
gboolean
open_url (const gchar *url, GError **error)
{
  gchar    *command;
  gboolean  result;

  g_debug ("Opening URL %s by using `xdg-open'.", url);

  command = g_strjoin (" ", "xdg-open", url, NULL);

  result = g_spawn_command_line_async (command, NULL);
  if (result == FALSE) {
    g_critical ("Failed to run `%s'.", command);
  }

  g_free (command);

  return result;
}
