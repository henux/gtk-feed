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

#include <gtk/gtk.h>

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
on_open_feed (GtkMenuItem *item,
              gpointer     user_data)
{
  g_debug ("on_open_feed -- not implemented\n");
}

static void
on_check_feeds (GtkMenuItem *item,
                gpointer     user_data)
{
  g_debug ("on_check_feeds -- not implemented\n");
}

static void
on_add_feed (GtkMenuItem *item,
             gpointer     user_data)
{
  g_debug ("on_add_feed -- not implemented\n");
}

static void
on_about (GtkMenuItem *item,
          gpointer     user_data)
{
  g_debug ("on_about -- not implemented\n");
}

static void
on_prefs (GtkMenuItem *item,
          gpointer     user_data)
{
  g_debug ("on_prefs -- not implemented\n");
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
  GtkMenuShell *menu;
  GtkWidget *item;

  menu = GTK_MENU_SHELL(gtk_menu_new ());
  g_assert (menu != NULL);

  item = gtk_menu_item_new_with_label ("Feeds go here...");
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);
  g_signal_connect (item, "activate", G_CALLBACK(on_open_feed), NULL);

  gtk_widget_show_all (GTK_WIDGET(menu));

  return GTK_MENU(menu);
}

static GtkMenu *
create_app_menu ()
{
  GtkMenuShell *menu;
  GtkWidget *item;

  menu = GTK_MENU_SHELL(gtk_menu_new ());
  g_assert (menu != NULL);

  item = gtk_menu_item_new_with_mnemonic ("_Check feeds");
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);
  g_signal_connect (item, "activate", G_CALLBACK(on_check_feeds), NULL);

  item = gtk_menu_item_new_with_mnemonic ("_Add feed");
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);
  g_signal_connect (item, "activate", G_CALLBACK(on_add_feed), NULL);

  item = gtk_separator_menu_item_new ();
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);

  item = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL);
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);
  g_signal_connect (item, "activate", G_CALLBACK(on_about), NULL);

  item = gtk_image_menu_item_new_from_stock (GTK_STOCK_PREFERENCES, NULL);
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);
  g_signal_connect (item, "activate", G_CALLBACK(on_prefs), NULL);

  item = gtk_separator_menu_item_new ();
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);

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
  GtkStatusIcon *icon;

  /* Initialize GTK and other libraries. */
  gtk_init (&argc, &argv);

  /* Create the menu objects.  */
  feed_menu = create_feed_menu ();
  g_assert (feed_menu != NULL);

  app_menu = create_app_menu ();
  g_assert (app_menu != NULL);

  /* Create the system tray icon and connect the signals. */
  icon = gtk_status_icon_new_from_file ("feedicon.png");
  g_assert (icon != NULL);

  g_signal_connect (icon, "activate", G_CALLBACK(on_activate), NULL);
  g_signal_connect (icon, "popup-menu", G_CALLBACK(on_popup_menu), NULL);

  /* Run the main loop. */
  gtk_main ();

  return 0;
}
