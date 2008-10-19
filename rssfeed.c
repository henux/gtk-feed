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

#include "rssfeed.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

/* Parser functions. */
static void
parse_item (xmlNodePtr root, RSSFeedItem *item)
{
  xmlNodePtr node;
  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      item->title = g_strdup ((const gchar *) xmlNodeGetContent (node));
    } else if (xmlStrcmp (node->name, (const xmlChar *) "link") == 0) {
      item->link = g_strdup ((const gchar *) xmlNodeGetContent (node));
    } else if (xmlStrcmp (node->name, (const xmlChar *) "description") == 0) {
      item->desc = g_strdup ((const gchar *) xmlNodeGetContent (node));
    }
  }
}

static void
parse_channel (xmlNodePtr root, RSSFeed *feed)
{
  xmlNodePtr node;
  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      feed->title = g_strdup ((const gchar *) xmlNodeGetContent (node));
    } else if (xmlStrcmp (node->name, (const xmlChar *) "link") == 0) {
      feed->link = g_strdup ((const gchar *) xmlNodeGetContent (node));
    } else if (xmlStrcmp (node->name, (const xmlChar *) "description") == 0) {
      feed->desc = g_strdup ((const gchar *) xmlNodeGetContent (node));
    } else if (xmlStrcmp (node->name, (const xmlChar *) "item") == 0) {
      RSSFeedItem *item;
      item = g_new0 (RSSFeedItem, 0);
      g_assert (item != NULL);
      feed->items = g_list_append (feed->items, item);
      parse_item (root, item);
    }
  }
}

static void
parse_rss (xmlNodePtr root, RSSFeed *feed)
{
  xmlNodePtr node;
  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "channel") == 0) {
      parse_channel (node, feed);
      break;
    }
  }
}

/* RSS feed parser front-end. */
RSSFeed *
rss_feed_new ()
{
  RSSFeed *feed;
  feed = g_new0 (RSSFeed, 1);
  g_assert (feed != NULL);
  return feed;
}

void
rss_feed_free (RSSFeed *feed)
{
  GList *node;
  g_assert (feed != NULL);
  g_free (feed->title);
  g_free (feed->link);
  g_free (feed->desc);
  for (node = feed->items; node != NULL; node = g_list_next (node)) {
    RSSFeedItem *item = (RSSFeedItem *) node->data;
    g_assert (item != NULL);
    g_free (item->title);
    g_free (item->link);
    g_free (item->desc);
  }
  g_free (feed);
}

gboolean
rss_feed_parse (RSSFeed     *feed,
                const gchar *uri)
{
  xmlDocPtr doc;
  xmlNodePtr node;

  doc = xmlReadFile (uri, NULL, 0);
  if (doc == NULL)
    return FALSE;

  node = xmlDocGetRootElement (doc);
  for (; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "rss") == 0) {
      parse_rss (node, feed);
      break;
    }
  }

  xmlFreeDoc (doc);

  return TRUE;
}
