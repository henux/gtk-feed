/*
Copyright (C) 2008 Henri Häkkinen,

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
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "callbacks.h"
#include "common.h"
#include "rssfeed.h"

static void
parse_item_element (xmlNodePtr     root,
                    RSSFeedParser *parser)
{
  xmlNodePtr  node;
  GtkWidget  *menu_item;

  g_assert (root != NULL);
  g_assert (parser != NULL);

  menu_item = gtk_menu_item_new ();

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      gchar     *title;
      GtkWidget *label;

      title = (gchar *) xmlNodeGetContent (node);
      g_strstrip (title);

      label = g_object_new (GTK_TYPE_LABEL,
                            "label", title,
                            "xalign", 0.0f,
                            NULL);

      gtk_container_add (GTK_CONTAINER(menu_item), label);
    } else if (xmlStrcmp (node->name, (const xmlChar *) "link") == 0) {
      gtk_widget_set_tooltip_text (menu_item,
                                   (const gchar *) xmlNodeGetContent (node));

      g_signal_connect (menu_item,
                        "activate",
                        G_CALLBACK(on_feed_open),
                        g_strdup ((const gchar *) xmlNodeGetContent (node)));
    }
  }

  gdk_threads_enter ();
  gtk_widget_show_all (menu_item);
  gtk_menu_shell_insert (GTK_MENU_SHELL(parser->feed_menu),
                         menu_item,
                         parser->next_pos++);
  gdk_threads_leave ();
}

static void
parse_channel_element (xmlNodePtr     root,
                       RSSFeedParser *parser)
{
  xmlNodePtr node;

  g_assert (root != NULL);
  g_assert (parser != NULL);

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "item") == 0) {
      parse_item_element (node, parser);
    }
  }
}

static void
parse_rss_element (xmlNodePtr     root,
                   RSSFeedParser *parser)
{
  xmlNodePtr node;

  g_assert (root != NULL);
  g_assert (parser != NULL);

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "channel") == 0) {
      parse_channel_element (node, parser);
      return;
    }
  }
}

gpointer
rss_feed_parser (RSSFeedParser *parser)
{
  xmlDocPtr   doc;
  xmlNodePtr  node;
  GtkWidget  *label;

  g_assert (parser != NULL);
  g_assert (parser->feed_uri != NULL);
  g_assert (parser->feed_menu != NULL);
  g_assert (parser->loading == NULL);

  doc = xmlReadFile (parser->feed_uri, NULL, 0);
  if (doc == NULL) {
    g_warning ("Failed to read %s", parser->feed_uri);
    goto cleanup;
  }

  g_debug ("Reading %s", parser->feed_uri);

  label = g_object_new (GTK_TYPE_LABEL,
                        "label", "<span style='italic'>Loading...</span>",
                        "use-markup", TRUE,
                        "xalign", 0.0f,
                        NULL);

  parser->loading = gtk_menu_item_new ();
  gtk_container_add (GTK_CONTAINER(parser->loading), label);

  gdk_threads_enter ();
  gtk_menu_shell_append (GTK_MENU_SHELL(parser->feed_menu),
                         parser->loading); 
  gtk_widget_show_all (parser->loading);
  gdk_threads_leave ();

  parser->next_pos = 0;

  for (node = xmlDocGetRootElement (doc); node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "rss") == 0) {
      parse_rss_element (node, parser);
      break;
    }
  }  

  g_debug ("Done reading %s", parser->feed_uri);

 cleanup:
  if (parser->loading) {
    gdk_threads_enter ();
    gtk_widget_destroy (parser->loading);
    gdk_threads_leave ();
  }

  xmlFreeDoc (doc);
  g_free (parser->feed_uri);
  g_free (parser);

  return NULL;
}
