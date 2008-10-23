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

#ifndef RSSFEED_H
#define RSSFEED_H

/*
 * RSS Feed Parser:
 *
 * The RSS feed parser works in a background thread.  Each RSS feed is read
 * by it's own thread; thus there will be an equal amount of parser threads
 * as there are feeds to read.
 *
 * Each parser gets a feed URI to locate the RSS XML file from, a menu item
 * for the feed corresponding to the one in the feeds menu, and a sub-menu
 * to build the feed links into.  The parser renames the menu item once it
 * knows the proper title of the feed and builds a submenu under it, where
 * each item in this submenu corresponds to a single feed article.
 *
 * Once the whole feed is read and processed, the background thread frees
 * all necessary resources and exists cleanly.
 */

typedef struct {
  gchar       *feed_uri;
  GtkMenuItem *item;
  GtkMenu     *submenu;
} RSSFeedParser;

gpointer rss_feed_parser (gpointer data);

#endif
