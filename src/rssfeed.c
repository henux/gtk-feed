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
#include "rssfeed.h"

static void
parse_item_element (xmlNodePtr  root,
                    GtkWidget  *menu)
{
  xmlNodePtr  node;
  GtkWidget  *item;

  g_assert (root != NULL);
  g_assert (menu != NULL);

  item = gtk_menu_item_new ();

  for (node = root->children; node != NULL; node = node->next) {
    if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      gchar     *title;
      GtkWidget *label;

      title = (gchar *) xmlNodeGetContent (node);
      g_strstrip (title);

      label = g_object_new (GTK_TYPE_LABEL,
                            "label", title,
                            "xalign", 0.0f,
                            NULL);

      gtk_container_add (GTK_CONTAINER(item), label);
    } else if (xmlStrcmp (node->name, (const xmlChar *) "link") == 0) {
      gtk_widget_set_tooltip_text (item,
                                   (const gchar *) xmlNodeGetContent (node));
 
      g_signal_connect (item,
                        "activate",
                        G_CALLBACK(on_feed_open),
                        g_strdup ((const gchar *) xmlNodeGetContent (node)));
    }
  }

  gdk_threads_enter ();
  gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
  gtk_widget_show_all (item);
  gdk_threads_leave ();
}

static void
parse_channel_element (xmlNodePtr  root,
                       GtkWidget  *menu)
{
  xmlNodePtr node;
  g_assert (root != NULL);
  g_assert (menu != NULL);
  for (node = root->children; node != NULL; node = node->next) {
    if (xmlStrcmp (node->name, (const xmlChar *) "item") == 0) {
      parse_item_element (node, menu);
    }
  }
}

static void
parse_rss_element (xmlNodePtr  root,
                   GtkWidget  *menu)
{
  xmlNodePtr node;
  g_assert (root != NULL);
  g_assert (menu != NULL);
  for (node = root->children; node != NULL; node = node->next) {
    if (xmlStrcmp (node->name, (const xmlChar *) "channel") == 0) {
      parse_channel_element (node, menu);
      break;
    }
  }
}

gpointer
rss_feed_parser (RSSFeedParser *parser)
{
  xmlDocPtr   doc;
  xmlNodePtr  node;
  GtkWidget  *menu;

  g_assert (parser != NULL);
  g_assert (parser->menu != NULL);
  g_assert (parser->source != NULL);

  menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM(parser->menu),
                             menu);
  
  doc = xmlReadFile (parser->source, NULL, 0);
  if (doc == NULL) {
    g_warning ("Failed to read %s", parser->source);
    goto cleanup;
  }

  g_debug ("Reading %s", parser->source);

  for (node = xmlDocGetRootElement (doc);
       node!= NULL;
       node = node->next) {
    if (xmlStrcmp (node->name, (const xmlChar *) "rss") == 0) {
      parse_rss_element (node, menu);
      break;
    }
  }  

  g_debug ("Done reading %s", parser->source);

 cleanup:
  xmlFreeDoc (doc);
  g_free (parser);
  return NULL;
}
