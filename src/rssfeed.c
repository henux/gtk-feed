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

static void
on_open_url (GtkMenuItem *item,
             gpointer     user_data)
{
  gchar *command;
  const gchar *url = (const gchar *) user_data;
  g_assert (url != NULL);

  g_debug ("Opening URL %s by using `xdg-open'.", url);

  command = g_strjoin (" ", "xdg-open", url, NULL);
  g_assert (command != NULL);

  if (!g_spawn_command_line_async (command, NULL)) {
    g_critical ("Failed to run `%s'.", command);
  }

  g_free (command);
}

static void
set_menu_title (GtkMenuItem *item,
                const gchar *title)
{
  GtkWidget *label;

  g_assert (item != NULL);
  g_assert (title != NULL);

  gdk_threads_enter ();

  label = gtk_bin_get_child (GTK_BIN(item));
  g_assert (label != NULL);
  gtk_label_set_text (GTK_LABEL(label), title);

  gdk_threads_leave ();
}

static void
set_menu_title_italic (GtkMenuItem *item,
                       const gchar *title)
{
  GtkWidget *label;
  gchar     *markup;

  g_assert (item != NULL);
  g_assert (title != NULL);

  gdk_threads_enter ();

  markup = g_strdup_printf ("<span style='italic'>%s</span>", title);

  label = gtk_bin_get_child (GTK_BIN(item));
  g_assert (label != NULL);

  gtk_label_set_markup (GTK_LABEL(label), markup);
  g_free (markup);

  gdk_threads_leave ();
}

/* Parser functions. */
static void
parse_item_element (xmlNodePtr  root,
                    GtkWidget  *menu)
{
  xmlNodePtr  node;
  GtkWidget  *item;
  xmlChar    *title = NULL;
  xmlChar    *link = NULL;

  g_assert (root != NULL);

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      /* Item title. */
      title = xmlNodeGetContent (node);
    } else if (xmlStrcmp (node->name, (const xmlChar *) "link") == 0) {
      /* Item link. */
      link = xmlNodeGetContent (node);
    }
  }

  if (title == NULL) {
    g_critical ("<item> element doesn't have <title> element.");
  } else if (link == NULL) {
    g_critical ("<item> element doesn't have <link> element.");
  } else {
    /* Create the menu item for the feed article. */

    gdk_threads_enter ();

    g_strstrip ((gchar *) title);
    item = gtk_menu_item_new_with_label ((const gchar *) title);
    g_assert (item != NULL);

    gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
    g_signal_connect (item, "activate", G_CALLBACK(on_open_url),
                      g_strdup ((const gchar *) link));

    gtk_widget_show (item);

    gdk_threads_leave ();
  }
}

static void
parse_channel_element (xmlNodePtr   root,
                       GtkMenuItem *submenu)
{
  xmlNodePtr  node;
  GtkWidget  *menu;
  xmlChar    *title = NULL;
  
  g_assert (root != NULL);

  /* Create the submenu for this feed channel. */
  gdk_threads_enter ();

  menu = gtk_menu_new ();
  g_assert (menu != NULL);
  gtk_menu_item_set_submenu (submenu, menu);
  gtk_widget_show (menu);

  gdk_threads_leave ();

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "title") == 0) {
      /* Channel title. */
      title = xmlNodeGetContent (node);
    } else if (xmlStrcmp (node->name, (const xmlChar *) "item") == 0) {
      /* Channel item. */
      parse_item_element (node, menu);
    }
  }

  if (title == NULL) {
    set_menu_title_italic (submenu, "Invalid RSS feed");
    g_critical ("<channel> element doesn't have <title> element.");
  } else {
    set_menu_title (submenu, title);
  }
}

static void
parse_rss_element (xmlNodePtr   root,
                   GtkMenuItem *submenu)
{
  xmlNodePtr node;

  g_assert (root != NULL);

  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "channel") == 0) {
      parse_channel_element (node, submenu);
      return;
    }
  }

  set_menu_title_italic (submenu, "Invalid RSS feed");
  g_critical ("<channel> element not found.");
}

static void
parse_doc (xmlDocPtr    doc,
           GtkMenuItem *submenu)
{
  xmlNodePtr node;

  g_assert (doc != NULL);

  for (node = xmlDocGetRootElement (doc); node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "rss") == 0) {
      parse_rss_element (node, submenu);
      return;
    }
  }  

  set_menu_title_italic (submenu, "Invalid RSS feed");
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
  g_assert (parser->submenu != NULL);

  g_debug ("Started to read %s.", parser->feed_uri);
  
  /* Parse the XML file into DOM tree. */
  doc = xmlReadFile (parser->feed_uri, NULL, 0);
  if (doc == NULL) {
    set_menu_title_italic (parser->submenu, "Invalid feed URL");
    g_warning ("Couldn't read %s.", parser->feed_uri);
    goto cleanup;
  }

  /* Process the DOM tree. */
  parse_doc (doc, parser->submenu);
  g_debug ("Finished reading %s.", parser->feed_uri);

  /* Cleanup. */
 cleanup:
  xmlFreeDoc (doc);
  g_free (parser->feed_uri);
  g_free (parser);
  
  return NULL;
}
