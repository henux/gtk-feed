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

#ifndef FEEDS_H
#define FEEDS_H

#include <gtk/gtk.h>

/*
 * Web feeds are Internet resources which contain news articles.  Each news
 * article contains a title, a description and a link to the web page
 * containing the full story.  Many web feeds also contain additional
 * information, but these are ignored.
 *
 * In practice, web feeds are XML documents.  Currently, there are three
 * different web feed standards in use: RSS 0.91, RSS/RDF 1.0 and Atom.  At
 * the moment, gtk-feed tries only to handle RSS 0.91 and it's variants.
 */

/*
 * Web feed structure.
 */
typedef struct {
  gchar    *title;      /* feed's title */
  gchar    *source;     /* feed's URL */
  gboolean  dirty;      /* if TRUE, the feed needs resynching */
  GtkMenu  *menu;       /* feed's submenu */
} Feed;

/*
 * List of feeds.
 */
extern GList *feeds;

/*
 * Loads the feed sources which the user has configured from the file
 * '$XDG_CONFIG/gtk-feed/feeds.xml' and builds the corresponding data
 * structures to 'feeds' list.  This function also calls sync_feeds to
 * automatically synchronize them.
 */
void load_feeds ();

/*
 * Saves the user configured feed data structures to the file
 * '$XDG_CONFIG/gtk-feed/feeds.xml'.
 */
void save_feeds ();

/*
 * Synchronises feeds which are marked as "dirty" by loading them from the
 * Internet.  This function will spawn background threads to synchronize
 * the feeds.
 */
void sync_feeds ();

/*
 * Flushes all feeds, marking them dirty and forcing them to be resynched
 * in the next sync_feeds call.
 */
void flush_feeds ();

#endif
