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

#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtklabel.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "common.h"
#include "rssfeed.h"

/* Parser functions. */
static void
parse_item_element (xmlNodePtr root,
                    GtkMenu   *submenu)
{
  xmlNodePtr  node;
  GtkWidget  *item;
  gchar      *title = NULL;
  gchar      *link = NULL;

  g_assert (root != NULL);

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      title = (gchar *) xmlNodeGetContent (node);
    } else if (xmlStrcmp (node->name, (const xmlChar *) "link") == 0) {
      link = (gchar *) xmlNodeGetContent (node);
    }
  }

  if (title == NULL) {
    g_critical ("<item> element doesn't have <title> element.");
  } else if (link == NULL) {
    g_critical ("<item> element doesn't have <link> element.");
  } else {
    g_strstrip (title);
    g_strstrip (link);
    add_menu_item_link (submenu, title, link);
  }
}

static void
parse_channel_element (xmlNodePtr   root,
                       GtkMenuItem *item,
                       GtkMenu     *submenu)
{
  xmlNodePtr  node;
  GtkWidget  *menu;
  gchar      *title = NULL;
  
  g_assert (root != NULL);

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      title = (gchar *) xmlNodeGetContent (node);
    } else if (xmlStrcmp (node->name, (const xmlChar *) "item") == 0) {
      parse_item_element (node, submenu);
    }
  }

  if (title == NULL) {
    set_menu_item_italic (item, "No title");
    g_critical ("<channel> element doesn't have <title> element.");
  } else {
    set_menu_item (item, title);
  }
}

static void
parse_rss_element (xmlNodePtr   root,
                   GtkMenuItem *item,
                   GtkMenu     *submenu)
{
  xmlNodePtr node;

  g_assert (root != NULL);

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "channel") == 0) {
      parse_channel_element (node, item, submenu);
      return;
    }
  }

  set_menu_item_italic (submenu, "Invalid RSS feed");
  g_critical ("<channel> element not found.");
}

static void
parse_doc (xmlDocPtr    doc,
           GtkMenuItem *item,
           GtkMenu     *submenu)
{
  xmlNodePtr node;

  g_assert (doc != NULL);

  for (node = xmlDocGetRootElement (doc); node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "rss") == 0) {
      parse_rss_element (node, item, submenu);
      return;
    }
  }  

  set_menu_item_italic (submenu, "Invalid RSS feed");
  g_critical ("<rssl> element not found.");
}

gpointer
rss_feed_parser (gpointer data)
{
  RSSFeedParser *parser;
  xmlDocPtr      doc;
  
  parser = (RSSFeedParser *) data;
  g_assert (parser != NULL);
  g_assert (parser->feed_uri != NULL);
  g_assert (parser->item != NULL);
  g_assert (parser->submenu != NULL);

  g_debug ("Started to read %s.", parser->feed_uri);
  
  /* Parse the XML file into DOM tree. */
  doc = xmlReadFile (parser->feed_uri, NULL, 0);
  if (doc == NULL) {
    set_menu_item_italic (parser->item, "Invalid feed URL");
    g_warning ("Couldn't read %s.", parser->feed_uri);
    goto cleanup;
  }

  /* Process the DOM tree. */
  parse_doc (doc, parser->item, parser->submenu);
  g_debug ("Finished reading %s.", parser->feed_uri);

  /* Cleanup. */
 cleanup:
  xmlFreeDoc (doc);
  g_free (parser->feed_uri);
  g_free (parser);
  
  return NULL;
}
