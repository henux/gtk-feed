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
    *dialog,
    *content_area, *action_area, *table,
    *url_label, *url_entry,
    *title_label, *title_entry;

  /* Subscribe dialog. */
  dialog = g_object_new (GTK_TYPE_DIALOG,
                         "title", "Subscribe",
                         "has-separator", FALSE,
                         "border-width", 12,
                         "resizable", FALSE,
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

  action_area = gtk_dialog_get_action_area (GTK_DIALOG(dialog));

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
