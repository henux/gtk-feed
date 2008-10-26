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

#include <glib/gthread.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "callbacks.h"
#include "common.h"
#include "rssfeed.h"

/* Creates the singleton status icon object.  Subsequent calls will return
   the same status icon object. */
GtkStatusIcon *
get_status_icon ()
{
  static GtkStatusIcon *status_icon = NULL;

  if (status_icon == NULL) {
    status_icon = gtk_status_icon_new_from_icon_name ("gtk-feed");

    g_signal_connect (status_icon,
                      "activate",
                      G_CALLBACK(on_icon_activate),
                      NULL);

    g_signal_connect (status_icon,
                      "popup-menu",
                      G_CALLBACK(on_icon_popup_menu),
                      NULL);
  }

  return status_icon;
}

/* Parses the <feed> element and it's child elements. */
static void
parse_feed_element (xmlNodePtr  root,
                    GtkWidget  *menu)
{
  xmlNodePtr  node;
  GtkWidget  *menu_item;

  g_assert (root != NULL);

  menu_item = gtk_menu_item_new ();
  gtk_menu_shell_append (GTK_MENU_SHELL(menu), menu_item);

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
    } else if (xmlStrcmp (node->name, (const xmlChar *) "icon-url") == 0) {
      /* TODO */
    } else if (xmlStrcmp (node->name, (const xmlChar *) "feed-url") == 0) {
      RSSFeedParser *parser;

      parser = g_new0 (RSSFeedParser, 1);
      parser->feed_uri = g_strdup ((const gchar *) xmlNodeGetContent (node));
      parser->feed_menu = gtk_menu_new ();

      gtk_menu_item_set_submenu (GTK_MENU_ITEM(menu_item),
                                 parser->feed_menu);

      if (g_thread_create ((GThreadFunc) rss_feed_parser, parser, FALSE, NULL) == NULL) {
        g_critical ("Failed to create parser thread for %s", parser->feed_uri);
        g_free (parser);
      }
    } else {
      g_message ("Skipping unknown element <%s>", node->name);
    }
  }
}

/* Parses the <feeds> element and it's child elements. */
static void
parse_feeds_element (xmlNodePtr  root,
                     GtkWidget  *menu)
{
  xmlNodePtr   node;
  g_assert (root != NULL);
  for (node = root->children; node != NULL; node = node->next) {
    if (node->type != XML_ELEMENT_NODE) {
      continue;
    } else if (xmlStrcmp (node->name, (const xmlChar *) "feed") == 0) {
      parse_feed_element (node, menu);
    } else {
      g_message ("Skipping unknown element <%s>", node->name);
    }
  }
}

/* Creates the singleton feeds menu object.  Subsequent calls will return
   the same menu object. */
GtkWidget *
get_feeds_menu ()
{
  static GtkWidget *feeds_menu = NULL;

  if (feeds_menu == NULL) {
    gchar      *filename;
    xmlDocPtr   doc;
    xmlNodePtr  node;

    feeds_menu = gtk_menu_new ();

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
    
    for (node = xmlDocGetRootElement (doc); node != NULL; node = node->next) {
      if (node->type != XML_ELEMENT_NODE) {
        continue;
      } else if (xmlStrcmp (node->name, (const xmlChar *) "feeds") == 0) {
        parse_feeds_element (node, feeds_menu);
        break;
      } else {
        g_message ("Skippping unknown element <%s>", node->name);
      }
    }

  cleanup:
    g_debug ("Done reading %s", filename);
    xmlFreeDoc (doc);
    g_free (filename);
    gtk_widget_show_all (feeds_menu);
  }
  
  return feeds_menu;
}

/* Creates the singleton main menu object.  Subsequent calls will return
   the same menu object. */
GtkWidget *
get_main_menu ()
{ 
  static GtkWidget *main_menu = NULL;

  if (main_menu == NULL) {
    GtkWidget *item, *image;

    main_menu = gtk_menu_new ();

    image = gtk_image_new_from_icon_name ("gtk-feed",
                                          GTK_ICON_SIZE_MENU);
    
    item = gtk_image_menu_item_new_with_mnemonic ("_Subscribe");
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(item),
                                   image);
    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);
    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_subscribe),
                      NULL);

    item = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);
    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_about),
                      NULL);

    item = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL(main_menu),
                           item);
    g_signal_connect (item,
                      "activate",
                      G_CALLBACK(on_main_quit),
                      NULL);

    gtk_widget_show_all (main_menu);
  }

  return main_menu;
}

/* Open the given URL in the system's web browser.  This is done by running
   the command `xdg-open URL'.  Returns TRUE if the operation succeeded, or
   FALSE if error occured.  The caller may opt to pass a pointer in the
   ERROR parameter to get more details. */
gboolean
open_url (const gchar *url, GError **error)
{
  gchar    *command;
  gboolean  result;

  g_debug ("Opening URL %s by using `xdg-open'.", url);

  command = g_strjoin (" ", "xdg-open", url, NULL);

  result = g_spawn_command_line_async (command, NULL);
  if (result == FALSE) {
    g_critical ("Failed to run `%s'.", command);
  }

  g_free (command);

  return result;
}
