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

#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "rssfeed.h"

void on_open_url (GtkMenuItem *, gpointer);

/* Parser functions. */
static GtkWidget *
parse_item_element (xmlNodePtr root)
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
      /* Item title. */
      g_free (title);
      title = g_strdup ((const gchar *) xmlNodeGetContent (node));
      g_strstrip (title);
    } else if (xmlStrcmp (node->name, (const xmlChar *) "link") == 0) {
      /* Item link. */
      g_free (link);
      link = g_strdup ((const gchar *) xmlNodeGetContent (node));
      g_strstrip (link);
    }
  }

  g_assert (title != NULL);
  g_assert (link != NULL);
  
  /* Create the menu item for this feed item. */
  item = gtk_menu_item_new_with_label (title);
  g_assert (item != NULL);  
  g_signal_connect (item, "activate", G_CALLBACK(on_open_url), link);

  g_free (title);
  
  return item;
}

static GtkWidget *
parse_channel_element (xmlNodePtr root)
{
  xmlNodePtr  node;
  GtkWidget  *menu;
  GtkWidget  *submenu;
  gchar      *title = NULL;
  
  g_assert (root != NULL);

  submenu = gtk_menu_new ();
  g_assert (submenu != NULL);

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      /* Channel title. */
      g_free (title);
      title = g_strdup ((const gchar *) xmlNodeGetContent (node));
    } else if (xmlStrcmp (node->name, (const xmlChar *) "item") == 0) {
      /* Channel item. */
      GtkWidget *subitem;
      subitem = parse_item_element (node);
      g_assert (subitem != NULL);
      gtk_menu_shell_append (GTK_MENU_SHELL(submenu), subitem);
    }
  }

  g_assert (title != NULL);

  /* Create a menu item for this feed. */
  menu = gtk_menu_item_new_with_label (title);
  g_assert (menu != NULL);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM(menu), submenu);

  g_free (title);
  
  return menu;
}

static GtkWidget *
parse_rss_element (xmlNodePtr root)
{
  xmlNodePtr node;

  g_assert (root != NULL);

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "channel") == 0) {
      return parse_channel_element (node);
    }
  }

  return NULL;
}

/* RSS feed parser front-end. */
GtkWidget *
parse_rss_feed (const gchar *uri)
{
  xmlDocPtr   doc;
  xmlNodePtr  node;
  GtkWidget  *menu = NULL;
  
  g_assert (uri != NULL);

  /* Parse the XML file into DOM tree. */
  doc = xmlReadFile (uri, NULL, 0);
  if (doc == NULL)
    return NULL;

  /* Process the DOM tree. */
  node = xmlDocGetRootElement (doc);
  for (; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "rss") == 0) {
      menu = parse_rss_element (node);
      break;
    }
  }

  /* Cleanup. */
  xmlFreeDoc (doc);

  return menu;
}
