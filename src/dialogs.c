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
                         "logo", get_app_logo (),
                         "version", PACKAGE_VERSION,
                         "wrap-license", TRUE,
                         "website", website,
                         NULL);
}

/***** SUBSCRIBE DIALOG *****/

/* The subscribe dialog instance. */
static GtkDialog *subscribe_dialog = NULL;

static void
on_subscribe_response (GtkDialog *dialog,
                       gint       response_id,
                       gpointer   user_data)
{
  g_debug ("Add feed response: %d", response_id);
  subscribe_dialog = NULL;
}

/* Shows the subscribe dialog. */
void
show_subscribe_dialog ()
{
  GtkWidget *dialog, *content, *label, *entry;

  if (subscribe_dialog != NULL)
    return;

  dialog = gtk_dialog_new_with_buttons ("Subscribe",
                                        NULL,
                                        0,
                                        GTK_STOCK_CANCEL,
                                        GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OK,
                                        GTK_RESPONSE_OK,
                                        NULL);

  g_signal_connect (dialog, "response", G_CALLBACK(on_subscribe_response), NULL);
  
  content = gtk_dialog_get_content_area (GTK_DIALOG(dialog));

  label = gtk_label_new ("Enter the website URL:");
  gtk_box_pack_start (GTK_BOX(content), label, TRUE, TRUE, 0);

  entry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX(content), entry, TRUE, TRUE, 0);

  subscribe_dialog = GTK_DIALOG(dialog);
  gtk_widget_show_all (dialog);
}
