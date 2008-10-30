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

#include "dialogs.h"
#include "common.h"

/***** ABOUT DIALOG *****/

/* List of authors.  */
static const gchar *authors[] = {
  "Henri Häkkinen <henux@users.sourcefore.net>",
  NULL
};

/* Program comments. */
static const char *comments = 
  "A lightweight GTK+ 2.0 RSS feed reader.";

/* Copyright notice, */
static const gchar *copyright =
  "Copyright (C) 2008 Henri Häkkinen";  

/* License notice. */
static const gchar *license =
  "This program is free software: you can redistribute it and/or modify" \
  "it under the terms of the GNU General Public License as published by" \
  "the Free Software Foundation, either version 3 of the License, or"   \
  "(at your option) any later version.\n\n"                             \
  "This program is distributed in the hope that it will be useful,"     \
  "but WITHOUT ANY WARRANTY; without even the implied warranty of"      \
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"       \
  "GNU General Public License for more details.\n\n"                    \
  "You should have received a copy of the GNU General Public License"   \
  "along with this program.  If not, see <http://www.gnu.org/licenses/>.";

/* Project website. */
static const gchar *website =
  "http://henux.nor.fi/coding/projects/15-gtk/24-gtk-feed";

/* Shows the about dialog. */
void
show_about_dialog ()
{
  gtk_show_about_dialog (NULL,
                         "authors", authors,
                         "comments", comments,
                         "copyright", copyright,
                         "license", license,
                         "logo-icon-name", "gtk-feed",
                         "version", PACKAGE_VERSION,
                         "wrap-license", TRUE,
                         "website", website,
                         NULL);
}

/***** SUBSCRIBE DIALOG *****/

/* Subscibe dialog response handler. */
static void
on_subscribe_response (GtkDialog *dialog,
                       gint       response_id,
                       gpointer   user_data)
{
  /* TODO */
}

/* Shows the subscribe dialog. */
void
show_subscribe_dialog ()
{
  GtkWidget
    *dialog, *content_area, *table,
    *url_label, *url_entry,
    *title_label, *title_entry;

  /* Subscribe dialog. */
  dialog = g_object_new (GTK_TYPE_DIALOG,
                         "title", "Subscribe",
                         "has-separator", FALSE,
                         "border-width", 12,
                         "resizable", FALSE,
                         "skip-taskbar-hint", TRUE,
                         NULL);

  gtk_dialog_add_buttons (GTK_DIALOG(dialog),
                          GTK_STOCK_CANCEL,
                          GTK_RESPONSE_CANCEL,
                          GTK_STOCK_OK,
                          GTK_RESPONSE_OK,
                          NULL);
    
  g_signal_connect (dialog,
                    "response",
                    G_CALLBACK(on_subscribe_response),
                    NULL);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
  g_object_set (content_area, "spacing", 12, NULL);

  /* Table layout. */
  table = g_object_new (GTK_TYPE_TABLE,
                        "n-columns", 2,
                        "n-rows", 2,
                        "column-spacing", 12,
                        NULL);

  gtk_box_pack_start (GTK_BOX(content_area), table, TRUE, TRUE, 0);
  
  /* Title label. */
  title_label = g_object_new (GTK_TYPE_LABEL,
                              "label", "Title:",
                              "xalign", 0.0f,
                              NULL);

  gtk_table_attach_defaults (GTK_TABLE(table), title_label, 0, 1, 0, 1);

  /* Title entry. */
  title_entry = g_object_new (GTK_TYPE_ENTRY, NULL);

  gtk_table_attach_defaults (GTK_TABLE(table), title_entry, 1, 2, 0, 1);

  /* Location label. */
  url_label = g_object_new (GTK_TYPE_LABEL,
                            "label", "Location:",
                            "xalign", 0.0f,
                            NULL);

  gtk_table_attach_defaults (GTK_TABLE(table), url_label, 0, 1, 1, 2);

  /* Location entry. */
  url_entry = g_object_new (GTK_TYPE_ENTRY, NULL);

  gtk_table_attach_defaults (GTK_TABLE(table), url_entry, 1, 2, 1, 2);

  /* Show the dialog. */
  gtk_widget_show_all (dialog);
}

/***** FEEDS DIALOG *****/

/* Columns. */
enum {
  FEEDS_TITLE_COLUMN,
  FEEDS_N_COLUMNS,
};

/* Feeds dialog response handler. */
static void
on_feeds_response (GtkDialog *dialog,
                   gint       response_id,
                   gpointer   user_data)
{
  /* TODO */
}

static GtkWidget *
build_feeds_list ()
{
  GtkWidget         *feeds_view;
  GtkListStore      *feeds_store;
  GtkCellRenderer   *title_renderer;
  GtkTreeViewColumn *title_column;
  GtkTreeIter        iter;

  feeds_store = gtk_list_store_new (FEEDS_N_COLUMNS,
                                    G_TYPE_STRING);

  gtk_list_store_append (feeds_store, &iter);
  gtk_list_store_set (feeds_store, &iter, 0, "Foo", -1);

  gtk_list_store_append (feeds_store, &iter);
  gtk_list_store_set (feeds_store, &iter, 0, "Baz", -1);

  feeds_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(feeds_store));
  g_object_unref (feeds_store);

  title_renderer = gtk_cell_renderer_text_new ();

  title_column = gtk_tree_view_column_new_with_attributes ("Feed",
                                                           title_renderer,
                                                           "text",
                                                           FEEDS_TITLE_COLUMN,
                                                           NULL);

  gtk_tree_view_append_column (GTK_TREE_VIEW(feeds_view), title_column);

  return feeds_view;
}

/* Shows the feeds dialog. */
void
show_feeds_dialog ()
{
  GtkWidget *dialog, *content_area, *feeds_list;

  /* Feeds dialog. */
  dialog = g_object_new (GTK_TYPE_DIALOG,
                         "title", "Feeds",
                         "has-separator", FALSE,
                         "border-width", 12,
                         "resizable", FALSE,
                         "skip-taskbar-hint", TRUE,
                         "default-height", 400,
                         NULL);

  gtk_dialog_add_buttons (GTK_DIALOG(dialog),
                          GTK_STOCK_ADD,
                          0,
                          GTK_STOCK_EDIT,
                          0,
                          GTK_STOCK_DELETE,
                          0,
                          GTK_STOCK_OK,
                          GTK_RESPONSE_OK,
                          NULL);

  g_signal_connect (dialog,
                    "response",
                    G_CALLBACK(on_feeds_response),
                    NULL);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
  g_object_set (content_area, "spacing", 12, NULL);

  /* Feeds list. */
  feeds_list = build_feeds_list ();

  gtk_box_pack_start (GTK_BOX(content_area), feeds_list, TRUE, TRUE, 0);

  /* Show the dialog. */
  gtk_widget_show_all (dialog);
}
