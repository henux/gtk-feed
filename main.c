/*
Copyright (C) 2008 Henri Hakkinen

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

/* Structures. */
struct MenuPos {
  gint     x, y;
  gboolean push_in;
};

/* Menu objects. */
static GtkMenu *rss_menu = NULL;
static GtkMenu *app_menu = NULL;

/* Callbacks. */
static void
menu_pos_func (GtkMenu  *menu,
               gint     *x,
               gint     *y,
               gboolean *push_in,
               gpointer  user_data)
{
  struct MenuPos *pos = (struct MenuPos *) user_data;
  g_assert (pos != NULL);
  *x = pos->x;
  *y = pos->y;
  *push_in = pos->push_in;
}

/* Icon event handlers. */
static void
on_activate (GtkStatusIcon *icon,
             gpointer       user_data)
{
  struct MenuPos pos;
  guint32 time;

  g_assert (icon != NULL);
  g_assert (rss_menu != NULL);

  gtk_status_icon_position_menu (rss_menu, &pos.x, &pos.y, &pos.push_in, icon);  
  time = gtk_get_current_event_time ();
  gtk_menu_popup (rss_menu, NULL, NULL, menu_pos_func, &pos, 0, time);
}

static void
on_popup_menu (GtkStatusIcon *icon,
               guint          button,
               guint          activate_time,
               gpointer       user_data)
{
  struct MenuPos pos;

  g_assert (icon != NULL);
  g_assert (app_menu != NULL);
  
  gtk_status_icon_position_menu (app_menu, &pos.x, &pos.y, &pos.push_in, icon);  
  gtk_menu_popup (app_menu, NULL, NULL, menu_pos_func, &pos, button, activate_time);
}

/* Menu event handlers. */
static void
on_reload_feeds (GtkMenuItem *item,
                 gpointer     user_data)
{
  g_debug ("on_reload_feeds -- not implemented\n");
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
create_rss_menu ()
{
  GtkMenuShell *menu;
  GtkWidget *item;

  menu = GTK_MENU_SHELL(gtk_menu_new ());
  g_assert (menu != NULL);

  item = gtk_menu_item_new_with_label ("FOO");
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);

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

  item = gtk_menu_item_new_with_mnemonic ("_Reload feeds");
  g_assert (item != NULL);
  gtk_menu_shell_append (menu, item);
  g_signal_connect (item, "activate", G_CALLBACK(on_reload_feeds), NULL);

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
  rss_menu = create_rss_menu ();
  g_assert (rss_menu != NULL);

  app_menu = create_app_menu ();
  g_assert (app_menu != NULL);

  /* Create the system tray icon and connect the signals. */
  icon = gtk_status_icon_new_from_stock (GTK_STOCK_HOME);
  g_assert (icon != NULL);

  g_signal_connect (icon, "activate", G_CALLBACK(on_activate), NULL);
  g_signal_connect (icon, "popup-menu", G_CALLBACK(on_popup_menu), NULL);

  /* Run the main loop. */
  gtk_main ();

  return 0;
}
