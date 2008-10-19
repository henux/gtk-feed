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

#ifndef RSSFEED_H
#define RSSFEED_H

#include <glib.h>

typedef struct {
  gchar *title;
  gchar *link;
  gchar *desc;
} RSSFeedItem;

typedef struct {
  gchar *title;
  gchar *link;
  gchar *desc;
  GList *items;
} RSSFeed;

RSSFeed * rss_feed_new   ();
void      rss_feed_free  (RSSFeed *feed);
gboolean  rss_feed_parse (RSSFeed *feed, const gchar *uri);

#endif
