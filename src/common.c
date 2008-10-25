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
#include "rssfeed.h"

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
GtkMenu *
get_feeds_menu ()
{
  static GtkMenu *feeds_menu = NULL;

  if (feeds_menu == NULL) {
    FILE  *fp;
    gsize  feeds_num = 0;
    gchar *feeds_file;

    feeds_file = g_build_filename (g_get_user_config_dir(),
                                   PACKAGE,
                                   "feeds",
                                   NULL);
  
    g_debug ("Loading feed URLs from %s.", feeds_file);
  
    feeds_menu = GTK_MENU(gtk_menu_new ());
    gtk_widget_show (GTK_WIDGET(feeds_menu));

    /* Read URIs from the config and build the menu. */
    fp = fopen (feeds_file, "r");
    if (fp == NULL) {
      add_menu_item_italic (feeds_menu, "Invalid feeds file");
      g_critical ("Couldn't open file %s.", feeds_file);
      g_free (feeds_file);
      return feeds_menu;
    }

    g_free (feeds_file);

    while (1) {
      char           uri[100];
      RSSFeedParser *parser;
      GThread       *thread;

      /* Read an URI from the file and strip whitespaces. */
      if (fgets (uri, 100, fp) == NULL)
        break;
      g_strstrip (uri);

      /* If the line starts with # or is empty, skip it. */
      if (uri[0] == '#' || uri[0] == '\0')
        continue;

      /* Create a dummy menu item and submenu for this feed. */
      parser = g_new (RSSFeedParser, 1);

      parser->feed_uri = g_strdup (uri);
      parser->item = add_menu_item_italic (feeds_menu, "Loading...");
      parser->submenu = GTK_MENU(gtk_menu_new ());

      gtk_menu_item_set_submenu (parser->item, GTK_WIDGET(parser->submenu));
      gtk_widget_show (GTK_WIDGET(parser->submenu));

      /* Spawn a thread for reading the feed from the URI. */
      thread = g_thread_create (rss_feed_parser, parser, FALSE, NULL);
      if (thread == NULL) {
        g_critical ("Failed to spawn parser thread for %s.", uri);
      }
      
      ++feeds_num;
    }

    if (feeds_num == 0) {
      add_menu_item_italic (feeds_menu, "No feeds loaded");
    }
    
    fclose (fp);
  }
  
  return feeds_menu;
}

/* Creates the singleton main menu object.  Subsequent calls will return
   the same menu object. */
GtkMenu *
get_main_menu ()
{ 
  static GtkMenu *main_menu = NULL;

  if (main_menu == NULL) {
    GtkWidget *item, *image;

    main_menu = GTK_MENU(gtk_menu_new ());

    image = gtk_image_new_from_icon_name ("gtk-feed", GTK_ICON_SIZE_MENU);
    
    item = gtk_image_menu_item_new_with_mnemonic ("_Subscribe");
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(item),
                                   image);
    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);
    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_subscribe),
                      NULL);

    item = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);
    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_about),
                      NULL);

    item = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);
    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_quit),
                      NULL);

    gtk_widget_show_all (GTK_WIDGET(main_menu));
  }

  return GTK_MENU(main_menu);
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

/* Menu helpers */
GtkMenuItem *
add_menu_item (GtkMenu *menu, const gchar *title)
{
  GtkWidget *item;

  g_assert (menu != NULL);
  g_assert (title != NULL);

  //gdk_threads_enter ();
  
  item = gtk_menu_item_new_with_label (title);
  gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
  gtk_widget_show_all (item);

  //gdk_threads_leave ();
  
  return GTK_MENU_ITEM(item);
}

GtkMenuItem *
add_menu_item_italic (GtkMenu *menu, const gchar *title)
{
  GtkWidget *label, *item;
  gchar     *markup;

  g_assert (menu != NULL);
  g_assert (title != NULL);

  //gdk_threads_enter ();
  
  markup = g_strdup_printf ("<span style='italic'>%s</span>", title);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL(label), markup);
  g_free (markup);
  
  item = gtk_menu_item_new ();
  gtk_container_add (GTK_CONTAINER(item), label);

  gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
  gtk_widget_show_all (item);

  //gdk_threads_leave ();
  
  return GTK_MENU_ITEM(item);
}

GtkMenuItem *
add_menu_item_link (GtkMenu *menu, const gchar *title, const gchar *link)
{
  GtkWidget *item;

  g_assert (menu != NULL);
  g_assert (title != NULL);
  g_assert (link != NULL);

  //gdk_threads_enter ();

  item = gtk_menu_item_new_with_label (title);
  gtk_widget_set_tooltip_text (item, link);
  gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
  gtk_widget_show_all (item);

  g_signal_connect (item,
                    "activate",
                    G_CALLBACK(on_feed_open),
                    g_strdup (link));  

  //gdk_threads_leave ();
  
  return GTK_MENU_ITEM(item);
}

void
set_menu_item (GtkMenuItem *item,
               const gchar *title)
{
  GtkWidget *label;

  g_assert (item != NULL);
  g_assert (title != NULL);

  //gdk_threads_enter ();

  label = gtk_bin_get_child (GTK_BIN(item));
  g_assert (label != NULL);
  gtk_label_set_text (GTK_LABEL(label), title);

  gtk_widget_show_all (GTK_WIDGET(item));

  //gdk_threads_leave ();
}

void
set_menu_item_italic (GtkMenuItem *item,
                      const gchar *title)
{
  GtkWidget *label;
  gchar     *markup;

  g_assert (item != NULL);
  g_assert (title != NULL);

  //gdk_threads_enter ();

  markup = g_strdup_printf ("<span style='italic'>%s</span>", title);

  label = gtk_bin_get_child (GTK_BIN(item));
  g_assert (label != NULL);

  gtk_label_set_markup (GTK_LABEL(label), markup);
  g_free (markup);

  gtk_widget_show_all (GTK_WIDGET(item));

  //gdk_threads_leave ();
}
