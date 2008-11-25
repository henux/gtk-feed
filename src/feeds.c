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

#include "common.h"
#include "feeds.h"
#include "rssfeed.h"

GList *feeds = NULL;

static void
parse_feed_element (xmlNodePtr root)
{
  xmlNodePtr  node;
  Feed       *feed;
  GtkWidget  *item;

  g_assert (root != NULL);

  feed = g_new0 (Feed, 1);
  feed->dirty = TRUE;
  feed->menu = GTK_MENU(gtk_menu_new ());

  for (node = root->children;
       node!= NULL;
       node = node->next) {
    if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      feed->title = g_strdup ((gchar *) xmlNodeGetContent (node));
    } else if (xmlStrcmp (node->name, (const xmlChar *) "source") == 0) {
      feed->source = g_strdup ((gchar *) xmlNodeGetContent (node));
    }
  }

  item = gtk_menu_item_new_with_label (feed->title);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM(item),
                             GTK_WIDGET(feed->menu));
  gtk_menu_shell_append (GTK_MENU_SHELL(get_feeds_menu ()),
                         item);
  gtk_widget_show_all (item);
  feeds = g_list_append (feeds, feed);
}

static void
parse_feeds_element (xmlNodePtr root)
{
  xmlNodePtr node;
  g_assert (root != NULL);
  for (node = root->children;
       node!= NULL;
       node = node->next) {
    if (xmlStrcmp (node->name, (const xmlChar *) "feed") == 0) {
      parse_feed_element (node);
    }
  }
}

void
load_feeds ()
{
  gchar      *filename;
  xmlDocPtr   doc;
  xmlNodePtr  node;

  filename = g_build_filename (g_get_user_config_dir (),
                               PACKAGE,
                               "feeds.xml",
                               NULL);

  doc = xmlReadFile (filename, NULL, 0);
  if (doc == NULL) {
    g_critical ("Failed to read %s", filename);
    goto cleanup;
  }

  g_debug ("Reading %s", filename);

  for (node = xmlDocGetRootElement (doc);
       node!= NULL;
       node = node->next) {
    if (xmlStrcmp (node->name, (const xmlChar *) "feeds") == 0) {
      parse_feeds_element (node);
    } else {
      g_message ("Skippping unknown element <%s>", node->name);
    }
  }

  g_debug ("Done reading %s", filename);
  sync_feeds ();
  
 cleanup:
  xmlFreeDoc (doc);
  g_free (filename);
}

void
save_feeds ()
{
  /* TODO */
}

void
sync_feeds ()
{
  GList *ptr;
  for (ptr = g_list_first (feeds);
       ptr!= NULL;
       ptr = g_list_next (ptr)) {
    if (((Feed*)ptr->data)->dirty) {
      RSSFeedParser *parser;
      parser = g_new (RSSFeedParser, 1);
      parser->menu = ((Feed*)ptr->data)->menu;
      parser->source = ((Feed*)ptr->data)->source;
      g_thread_create ((GThreadFunc) rss_feed_parser, parser, FALSE, NULL);
      ((Feed*)ptr->data)->dirty = FALSE;
    }
  }
}

void
flush_feeds ()
{
  GList *ptr;
  for (ptr = g_list_first (feeds);
       ptr!= NULL;
       ptr = g_list_next (ptr)) {
    ((Feed*)ptr->data)->dirty = TRUE;
  }
}
