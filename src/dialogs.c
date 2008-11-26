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

#include "common.h"
#include "dialogs.h"
#include "feeds.h"

/* Closure notify callback to destroy the dialog data structure. */
static void
destroy_data (gpointer *data,
              GClosure *closure)
{
  g_free (data);
}

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

/* Subscribe dialog data structure. */
typedef struct {
  GtkEntry *title;
  GtkEntry *source;
} SubscribeDialog;

/* Subscibe dialog response handler. */
static void
on_subscribe_response (GtkDialog       *dialog,
                       gint             response_id,
                       SubscribeDialog *data)
{
  g_assert (dialog != NULL);
  g_assert (data != NULL);
  g_assert (data->title != NULL);
  g_assert (data->source != NULL);

  if (response_id == GTK_RESPONSE_OK) {
    Feed      *feed;

    feed = g_new (Feed, 1);

    feed->title  = g_strdup (gtk_entry_get_text (data->title));
    feed->source = g_strdup (gtk_entry_get_text (data->source));
    feed->dirty  = TRUE;
    feed->menu   = gtk_menu_item_new_with_label (feed->title);

    gtk_widget_show_all (feed->menu);

    feeds = g_list_append (feeds, feed);
    sync_feeds ();
  }

  gtk_widget_destroy (GTK_WIDGET(dialog));
}

/* Shows the subscribe dialog. */
void
show_subscribe_dialog ()
{
  SubscribeDialog *data;
  GtkWidget       *dialog;
  GtkWidget       *content;
  GtkWidget       *table;
  GtkWidget       *label;    

  /* Subscribe dialog. */
  data = g_new0 (SubscribeDialog, 1);

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

  g_signal_connect_data (dialog,
                         "response",
                         G_CALLBACK(on_subscribe_response),
                         data,
                         (GClosureNotify) destroy_data,
                         0);

  content = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
  g_object_set (content, "spacing", 12, NULL);

  /* Table layout. */
  table = g_object_new (GTK_TYPE_TABLE,
                        "n-columns", 2,
                        "n-rows", 2,
                        "column-spacing", 12,
                        NULL);
  gtk_box_pack_start (GTK_BOX(content), table, TRUE, TRUE, 0);
  
  /* Title label. */
  label = g_object_new (GTK_TYPE_LABEL,
                        "label", "Title:",
                        "xalign", 0.0f,
                        NULL);
  gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 0, 1);

  /* Title entry. */
  data->title = GTK_ENTRY(g_object_new (GTK_TYPE_ENTRY, NULL));
  gtk_table_attach_defaults (GTK_TABLE(table), GTK_WIDGET(data->title), 1, 2, 0, 1);

  /* Source label. */
  label = g_object_new (GTK_TYPE_LABEL,
                        "label", "Source:",
                        "xalign", 0.0f,
                        NULL);
  gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 1, 2);

  /* Source entry. */
  data->source = GTK_ENTRY(g_object_new (GTK_TYPE_ENTRY, NULL));
  gtk_table_attach_defaults (GTK_TABLE(table), GTK_WIDGET(data->source), 1, 2, 1, 2);

  /* Show the dialog. */
  gtk_widget_show_all (dialog);
}

/***** FEEDS DIALOG *****/

/* Feeds dialog data structure. */
typedef struct {
  GtkTreeView      *feeds;
  GtkTreeSelection *selection;
} FeedsDialog;

/* Columns. */
enum {
  FEEDS_PTR_COLUMN,
  FEEDS_TITLE_COLUMN,
  FEEDS_N_COLUMNS,
};

/* Feeds dialog response handler. */
static void
on_feeds_response (GtkDialog   *dialog,
                   gint         response_id,
                   FeedsDialog *data)
{
  g_assert (dialog != NULL);
  g_assert (data != NULL);
  g_assert (data->feeds != NULL);
  g_assert (data->selection != NULL);

  if (response_id == GTK_RESPONSE_OK) {
    /* The "Ok" button was pressed or the dialog otherwise closed. */
    gtk_widget_destroy (GTK_WIDGET(dialog));
  } else if (response_id == GTK_RESPONSE_YES) {
    /* The "Add" button was pressed. */
    show_subscribe_dialog ();
  } else if (response_id == GTK_RESPONSE_NO) {
    /* The "Delete" button was pressed. */
    GtkTreeModel *model;
    GtkTreeIter   iter;

    if (gtk_tree_selection_get_selected (data->selection, &model, &iter)) {
      Feed *feed;

      gtk_tree_model_get (model, &iter, 0, (gpointer) &feed, -1);
      g_assert (feed != NULL);
      gtk_list_store_remove (GTK_LIST_STORE(model), &iter);
      feeds = g_list_remove (feeds, feed);

      gtk_widget_destroy (GTK_WIDGET(feed->menu));
      g_free (feed->title);
      g_free (feed->source);
      g_free (feed);
    }
  }
}

/* Populate the feeds list. */
static GtkWidget *
build_feeds_list ()
{
  GtkWidget         *feeds_view;
  GtkListStore      *feeds_store;
  GtkCellRenderer   *title_renderer;
  GtkTreeViewColumn *title_column;
  GList             *ptr;
  GtkTreeIter        iter;

  feeds_store =
    gtk_list_store_new (FEEDS_N_COLUMNS,
                        G_TYPE_POINTER,
                        G_TYPE_STRING);

  for (ptr = g_list_first (feeds);
       ptr!= NULL;
       ptr = g_list_next (ptr)) {
    gtk_list_store_append (feeds_store, &iter);
    gtk_list_store_set (feeds_store,
                        &iter,
                        0, ptr->data, 
                        1, ((Feed*)ptr->data)->title,
                        -1);
  }

  feeds_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(feeds_store));
  g_object_unref (feeds_store);

  title_renderer = gtk_cell_renderer_text_new ();

  title_column =
    gtk_tree_view_column_new_with_attributes ("Feed",
                                              title_renderer,
                                              "text",
                                              FEEDS_TITLE_COLUMN,
                                              NULL);

  gtk_tree_view_append_column (GTK_TREE_VIEW(feeds_view),
                               title_column);

  return feeds_view;
}

/* Shows the feeds dialog. */
void
show_feeds_dialog ()
{
  FeedsDialog *data;
  GtkWidget   *dialog;
  GtkWidget   *content;

  /* Feeds dialog. */
  data = g_new0 (FeedsDialog, 1);

  dialog = g_object_new (GTK_TYPE_DIALOG,
                         "title", "Feeds",
                         "has-separator", FALSE,
                         "border-width", 12,
                         "skip-taskbar-hint", TRUE,
                         "default-width", 300,
                         "default-height", 300,
                         NULL);

  gtk_dialog_add_buttons (GTK_DIALOG(dialog),
                          GTK_STOCK_ADD, GTK_RESPONSE_YES,
                          GTK_STOCK_DELETE, GTK_RESPONSE_NO,
                          GTK_STOCK_OK, GTK_RESPONSE_OK,
                          NULL);

  g_signal_connect_data (dialog,
                         "response",
                         G_CALLBACK(on_feeds_response),
                         data,
                         (GClosureNotify) destroy_data,
                         0);

  content = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
  g_object_set (content, "spacing", 12, NULL);

  /* Feeds list. */
  data->feeds = GTK_TREE_VIEW(build_feeds_list ());
  data->selection = gtk_tree_view_get_selection (data->feeds);

  gtk_box_pack_start (GTK_BOX(content),
                      GTK_WIDGET(data->feeds),
                      TRUE,
                      TRUE,
                      0);

  /* Show the dialog. */
  gtk_widget_show_all (dialog);
}
