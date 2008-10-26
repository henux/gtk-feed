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
 * Once the whole feed is read and processed, the background thread frees
 * all necessary resources and exists cleanly.
 */

typedef struct {
  gchar     *feed_uri;
  GtkWidget *feed_menu;
} RSSFeedParser;

gpointer rss_feed_parser (RSSFeedParser *parser);

#endif
