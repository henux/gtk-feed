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

#include <glib/gmem.h>
#include <glib/gmessages.h>
#include <glib/gstrfuncs.h>
#include <glib/gspawn.h>

#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtklabel.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

#include "common.h"

GdkPixbuf *icon_16x16 = NULL;
GdkPixbuf *icon_48x48 = NULL;

static void
on_open_url (GtkMenuItem *item,
             gpointer     user_data)
{
  gchar *command;
  const gchar *url = (const gchar *) user_data;
  g_assert (url != NULL);

  g_debug ("Opening URL %s by using `xdg-open'.", url);

  command = g_strjoin (" ", "xdg-open", url, NULL);
  g_assert (command != NULL);

  if (!g_spawn_command_line_async (command, NULL)) {
    g_critical ("Failed to run `%s'.", command);
  }

  g_free (command);
}

/* Menu helpers */
void
add_menu_item (GtkMenu *menu, const gchar *title)
{
  GtkWidget *item;

  g_assert (menu != NULL);
  g_assert (title != NULL);

  gdk_threads_enter ();

  item = gtk_menu_item_new_with_label (title);
  gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
  gtk_widget_show (item);

  gdk_threads_leave ();
}

void
add_menu_item_italic (GtkMenu *menu, const gchar *title)
{
  GtkWidget *label, *item;
  gchar     *markup;

  g_assert (menu != NULL);
  g_assert (title != NULL);

  gdk_threads_enter ();

  markup = g_strdup_printf ("<span style='italic'>%s</span>", title);
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL(label), markup);
  g_free (markup);
  
  item = gtk_menu_item_new ();
  gtk_container_add (GTK_CONTAINER(item), label);

  gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
  gtk_widget_show (item);

  gdk_threads_leave ();
}

void
add_menu_item_link (GtkMenu *menu, const gchar *title, const gchar *link)
{
  GtkWidget *item;

  g_assert (menu != NULL);
  g_assert (title != NULL);
  g_assert (link != NULL);

  gdk_threads_enter ();

  item = gtk_menu_item_new_with_label (title);
  gtk_widget_set_tooltip_text (item, link);
  gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
  gtk_widget_show (item);

  g_signal_connect (item, "activate", G_CALLBACK(on_open_url),
                    g_strdup (link));
  
  gdk_threads_leave ();
}

void
set_menu_item (GtkMenuItem *item,
               const gchar *title)
{
  GtkWidget *label;

  g_assert (item != NULL);
  g_assert (title != NULL);

  gdk_threads_enter ();

  label = gtk_bin_get_child (GTK_BIN(item));
  g_assert (label != NULL);
  gtk_label_set_text (GTK_LABEL(label), title);

  gdk_threads_leave ();
}

void
set_menu_item_italic (GtkMenuItem *item,
                      const gchar *title)
{
  GtkWidget *label;
  gchar     *markup;

  g_assert (item != NULL);
  g_assert (title != NULL);

  gdk_threads_enter ();

  markup = g_strdup_printf ("<span style='italic'>%s</span>", title);

  label = gtk_bin_get_child (GTK_BIN(item));
  g_assert (label != NULL);

  gtk_label_set_markup (GTK_LABEL(label), markup);
  g_free (markup);

  gdk_threads_leave ();
}

/* Filename helpers */
gchar *
get_pixmap_filename (const gchar *filename)
{
  return g_build_filename (DATADIR, PACKAGE_NAME, "pixmaps",
                           filename, NULL);
}

gchar *
get_config_filename (const gchar *filename)
{
  return g_build_filename (g_get_user_config_dir (), PACKAGE_NAME,
                           filename, NULL);
}
