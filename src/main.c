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

#include <glib/gmessages.h>
#include <glib/gstrfuncs.h>

#include <gtk/gtkmain.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkimagemenuitem.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkstatusicon.h>
#include <gtk/gtkstock.h>

#include "about.h"
#include "common.h"
#include "options.h"
#include "rssfeed.h"

/* Menu objects. */
static GtkMenu *feed_menu = NULL;
static GtkMenu *app_menu = NULL;

/* Icon event handlers. */
static void
on_activate (GtkStatusIcon *icon,
             gpointer       user_data)
{
  g_assert (icon != NULL);
  g_assert (feed_menu != NULL);

  gtk_menu_popup (feed_menu, NULL, NULL, gtk_status_icon_position_menu,
                  icon, 0, gtk_get_current_event_time ());
}

static void
on_popup_menu (GtkStatusIcon *icon,
               guint          button,
               guint          activate_time,
               gpointer       user_data)
{
  g_assert (icon != NULL);
  g_assert (app_menu != NULL);

  gtk_menu_popup (app_menu, NULL, NULL, gtk_status_icon_position_menu,
                  icon, button, activate_time);
}

/* Menu event handlers. */
static void
on_about (GtkMenuItem *item,
          gpointer     user_data)
{
  show_about_box ();
}

static void
on_quit (GtkMenuItem *item,
         gpointer     user_data)
{
  gtk_main_quit ();
}

/* Helper functions. */
static GtkMenu *
create_feed_menu (const char *feeds_file)
{
  FILE    *fp;
  GtkMenu *menu;
  gsize    feeds_num = 0;

  g_debug ("Loading feed URLs from %s.", feeds_file);
  
  menu = GTK_MENU(gtk_menu_new ());
  g_assert (menu != NULL);
  gtk_widget_show (GTK_WIDGET(menu));

  /* Read URIs from the config and build the menu. */
  fp = fopen (feeds_file, "r");
  if (fp == NULL) {
    add_menu_item_italic (menu, "Invalid feeds file");
    g_critical ("Couldn't open file %s.", feeds_file);
    return menu;
  }

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
    g_assert (parser != NULL);

    parser->feed_uri = g_strdup (uri);
    parser->item     = add_menu_item_italic (menu, "Loading...");
    parser->submenu  = GTK_MENU(gtk_menu_new ());

    gtk_menu_item_set_submenu (parser->item, GTK_WIDGET(parser->submenu));
    gtk_widget_show (GTK_WIDGET(parser->submenu));
    
    /* Spawn a thread for reading the feed from the URI. */
    thread = g_thread_create (rss_feed_parser, parser, FALSE, NULL);
    if (thread == NULL) {
      g_critical ("Failed to spawn a thread for %s.", uri);
    }

    ++feeds_num;
  }

  if (feeds_num == 0) {
    add_menu_item_italic (menu, "No feeds loaded");
  }

  fclose (fp);

  return menu;
}

static GtkMenu *
create_app_menu ()
{
  GtkMenuShell *menu;
  GtkWidget    *item;

  menu = GTK_MENU_SHELL(gtk_menu_new ());
  g_assert (menu != NULL);

  item = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL);
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);
  g_signal_connect (item, "activate", G_CALLBACK(on_about), NULL);

  item = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);
  g_signal_connect (item, "activate", G_CALLBACK(on_quit), NULL);

  gtk_widget_show_all (GTK_WIDGET(menu));

  return GTK_MENU(menu);
}

/* The program main. */
int
main (int argc, char **argv)
{
  gchar         *icon_16x16_file;
  gchar         *icon_48x48_file;
  GtkStatusIcon *icon;
  
  /* Initialize GTK and other libraries. */
  g_thread_init (NULL);
  gdk_threads_init ();
  gtk_init (&argc, &argv);

  /* General application initialization. */
  g_set_application_name ("GTK Feed Reader");

  /* Handle command line options. */
  if (!get_options (&argc, &argv))
    return 1;

  /* Build filenames. */
  icon_16x16_file = get_pixmap_filename ("feed-icon-16x16.png");
  icon_48x48_file = get_pixmap_filename ("feed-icon-48x48.png");

  if (options.feeds_file == NULL)
    options.feeds_file = get_config_filename ("feeds");

  /* Load icon pixbufs. */
  icon_16x16 = gdk_pixbuf_new_from_file (icon_16x16_file, NULL);
  icon_48x48 = gdk_pixbuf_new_from_file (icon_48x48_file, NULL);

  if (icon_16x16 == NULL)
    g_critical ("Couldn't open pixbuf from file %s.", icon_16x16_file);
  if (icon_48x48 == NULL)
    g_critical ("Couldn't open pixbuf from file %s.", icon_48x48_file);

  /* Create the menu objects.  */
  feed_menu = create_feed_menu (options.feeds_file);
  app_menu = create_app_menu ();

  /* Create the system tray icon and connect the signals. */
  icon = gtk_status_icon_new_from_pixbuf (icon_16x16);
  g_signal_connect (icon, "activate", G_CALLBACK(on_activate), NULL);
  g_signal_connect (icon, "popup-menu", G_CALLBACK(on_popup_menu), NULL);

  /* Free filenames before entering the main loop. */
  g_free (icon_16x16_file);
  g_free (icon_48x48_file);

  /* Run the main loop. */
  gtk_main ();

  return 0;
}
