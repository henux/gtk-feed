/*
Copyright (C) 2008 Henri Häkkinen

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

#include <gtk/gtkmain.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkimagemenuitem.h>
#include <gtk/gtkstatusicon.h>
#include <gtk/gtkstock.h>

#include "common.h"
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
  g_debug ("on_about -- not implemented");
}

static void
on_quit (GtkMenuItem *item,
         gpointer     user_data)
{
  gtk_main_quit ();
}

/* Helper functions. */
static GtkMenu *
create_feed_menu ()
{
  gchar        *feeds_file;
  FILE         *fp;
  GtkMenuShell *menu;

  /* Read URIs from the config and build the menu. */
  feeds_file = get_config_filename ("feeds");
  g_assert (feeds_file != NULL);

  fp = fopen (feeds_file, "r");
  if (fp == NULL) {
    g_critical ("%s not found", feeds_file);
    g_free (feeds_file);
    return NULL;
  }

  menu = GTK_MENU_SHELL(gtk_menu_new ());
  g_assert (menu != NULL);

  while (1) {
    char           uri[100];
    GtkWidget     *item;
    RSSFeedParser *parser;
    GThread       *thread;

    /* Read an URI from the file and strip whitespaces. */
    if (fgets (uri, 100, fp) == NULL)
      break;
    g_strstrip (uri);

    /* Create a dummy menu item for this feed. */
    item = gtk_menu_item_new_with_label ("Feed");
    g_assert (item != NULL);
    gtk_menu_shell_append (menu, item);
    gtk_widget_show (item);
    
    /* Spawn a thread for reading the feed from the URI. */
    parser = g_new (RSSFeedParser, 1);
    g_assert (parser != NULL);
    parser->feed_uri = g_strdup (uri);
    parser->submenu = GTK_MENU_ITEM(item);

    thread = g_thread_create (rss_feed_parser, parser, FALSE, NULL);
    if (thread == NULL) {
      g_critical ("failed to create a thread to read %s", uri);
    }
  }

  fclose (fp);
  g_free (feeds_file);

  gtk_widget_show (GTK_WIDGET(menu));

  return GTK_MENU(menu);
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
  gchar         *icon_file;
  GtkStatusIcon *icon;

  /* Initialize GTK and other libraries. */
  g_thread_init (NULL);
  gdk_threads_init ();
  gtk_init (&argc, &argv);

  /* Create the menu objects.  */
  feed_menu = create_feed_menu ();
  g_assert (feed_menu != NULL);

  app_menu = create_app_menu ();
  g_assert (app_menu != NULL);

  /* Create the system tray icon and connect the signals. */
  icon_file = get_pixmap_filename ("feed-icon-14x14.png");
  icon = gtk_status_icon_new_from_file (icon_file);
  g_assert (icon != NULL);
  g_free (icon_file);

  g_signal_connect (icon, "activate", G_CALLBACK(on_activate), NULL);
  g_signal_connect (icon, "popup-menu", G_CALLBACK(on_popup_menu), NULL);

  /* Run the main loop. */
  gtk_main ();

  return 0;
}
