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

#include <glib.h>
#include <string.h>
#include "rssfeed.h"

/* Parser structure declarations. */
static GMarkupParser item_title_parser;
static GMarkupParser item_link_parser;
static GMarkupParser item_description_parser;

static GMarkupParser channel_title_parser;
static GMarkupParser channel_link_parser;
static GMarkupParser channel_description_parser;

static GMarkupParser item_parser;
static GMarkupParser channel_parser;
static GMarkupParser rss_parser;

/* Subparser event handlers. */
static void
item_title_text (GMarkupParseContext *context,
                 const gchar         *text,
                 gsize                text_len,
                 gpointer             user_data,
                 GError             **error)
{
  RSSFeedItem *item = (RSSFeedItem *) user_data;
  g_assert (item != NULL);
  item->title = g_string_append_len (item->title, text, text_len);
}

static void
item_link_text (GMarkupParseContext *context,
                const gchar         *text,
                gsize                text_len,
                gpointer             user_data,
                GError             **error)
{
  RSSFeedItem *item = (RSSFeedItem *) user_data;
  g_assert (item != NULL);
  item->link = g_string_append_len (item->link, text, text_len);
}

static void
item_description_text (GMarkupParseContext *context,
                       const gchar         *text,
                       gsize                text_len,
                       gpointer             user_data,
                       GError             **error)
{
  RSSFeedItem *item = (RSSFeedItem *) user_data;
  g_assert (item != NULL);
  item->desc = g_string_append_len (item->desc, text, text_len);
}

static void
channel_title_text (GMarkupParseContext *context,
                    const gchar         *text,
                    gsize                text_len,
                    gpointer             user_data,
                    GError             **error)
{
  RSSFeed *feed = (RSSFeed *) user_data;
  g_assert (feed != NULL);
  feed->title = g_string_append_len (feed->title, text, text_len);
}

static void
channel_link_text (GMarkupParseContext *context,
                   const gchar         *text,
                   gsize                text_len,
                   gpointer             user_data,
                   GError             **error)
{
  RSSFeed *feed = (RSSFeed *) user_data;
  g_assert (feed != NULL);
  feed->link = g_string_append_len (feed->link, text, text_len);
}

static void
channel_description_text (GMarkupParseContext *context,
                          const gchar         *text,
                          gsize                text_len,
                          gpointer             user_data,
                          GError             **error)
{
  RSSFeed *feed = (RSSFeed *) user_data;
  g_assert (feed != NULL);
  feed->desc = g_string_append_len (feed->desc, text, text_len);
}

static void
item_start_element (GMarkupParseContext *context,
                    const gchar         *element_name,
                    const gchar        **attrib_names,
                    const gchar        **attrib_values,
                    gpointer             user_data,
                    GError             **error)
{
  if (strcmp (element_name, "title") == 0) {
    g_markup_parse_context_push (context, &item_title_parser, NULL);
  } else if (strcmp (element_name, "link") == 0) {
    g_markup_parse_context_push (context, &item_link_parser, NULL);
  } else if (strcmp (element_name, "description") == 0) {
    g_markup_parse_context_push (context, &item_description_parser, NULL);
  }
}

static void
item_end_element (GMarkupParseContext *context,
                  const gchar         *element_name,
                  gpointer             user_data,
                  GError             **error)
{
  if (strcmp (element_name, "title") == 0 ||
      strcmp (element_name, "link") == 0 ||
      strcmp (element_name, "description") == 0)
    {
      g_markup_parse_context_pop (context);
    }
}

static void
channel_start_element (GMarkupParseContext *context,
                       const gchar         *element_name,
                       const gchar        **attrib_names,
                       const gchar        **attrib_values,
                       gpointer             user_data,
                       GError             **error)
{
  if (strcmp (element_name, "title") == 0) {
    g_markup_parse_context_push (context, &channel_title_parser, NULL);
  } else if (strcmp (element_name, "link") == 0) {
    g_markup_parse_context_push (context, &channel_link_parser, NULL);
  } else if (strcmp (element_name, "description") == 0) {
    g_markup_parse_context_push (context, &channel_description_parser, NULL);
  } else if (strcmp (element_name, "item") == 0) {
    g_markup_parse_context_push (context, &item_parser, NULL);
  }
}

static void
channel_end_element (GMarkupParseContext *context,
                     const gchar         *element_name,
                     gpointer             user_data,
                     GError             **error)
{
  if (strcmp (element_name, "title") == 0 ||
      strcmp (element_name, "link") == 0 ||
      strcmp (element_name, "description") == 0 ||
      strcmp (element_name, "item") == 0)
    {
      g_markup_parse_context_pop (context);
    }
}

static void
rss_start_element (GMarkupParseContext *context,
                   const gchar         *element_name,
                   const gchar        **attrib_names,
                   const gchar        **attrib_values,
                   gpointer             user_data,
                   GError             **error)
{
  if (strcmp (element_name, "channel") == 0) {
    g_markup_parse_context_push (context, &channel_parser, NULL);
  }
}

static void
rss_end_element (GMarkupParseContext *context,
                 const gchar         *element_name,
                 gpointer             user_data,
                 GError             **error)
{
  if (strcmp (element_name, "channel") == 0) {
    g_markup_parse_context_pop (context);
  }
}

static void
main_start_element (GMarkupParseContext *context,
                    const gchar         *element_name,
                    const gchar        **attrib_names,
                    const gchar        **attrib_values,
                    gpointer             user_data,
                    GError             **error)
{
  if (strcmp (element_name, "rss") == 0) {
    g_markup_parse_context_push (context, &rss_parser, NULL);
  }
}

static void
main_end_element (GMarkupParseContext *context,
                  const gchar         *element_name,
                  gpointer             user_data,
                  GError             **error)
{
  if (strcmp (element_name, "rss") == 0) {
    g_markup_parse_context_pop (context);
  }
}

/* Subparser structures definitions. */
static GMarkupParser item_title_parser = {
  NULL, NULL, item_title_text, NULL, NULL
};

static GMarkupParser item_link_parser = {
  NULL, NULL, item_link_text, NULL, NULL
};

static GMarkupParser item_description_parser = {
  NULL, NULL, item_description_text, NULL, NULL
};

static GMarkupParser channel_title_parser = {
  NULL, NULL, channel_title_text, NULL, NULL
};

static GMarkupParser channel_link_parser = {
  NULL, NULL, channel_link_text, NULL, NULL
};

static GMarkupParser channel_description_parser = {
  NULL, NULL, channel_description_text, NULL, NULL
};

static GMarkupParser item_parser = {
  item_start_element, item_end_element, NULL, NULL, NULL
};

static GMarkupParser channel_parser = {
  channel_start_element, channel_end_element, NULL, NULL, NULL
};

static GMarkupParser rss_parser = {
  rss_start_element, rss_end_element, NULL, NULL, NULL
};

static GMarkupParser main_parser = {
  main_start_element, main_end_element, NULL, NULL, NULL
};

/* RSS feed parser front-end. */
RSSFeed *
rss_feed_new ()
{
  RSSFeed *rss;

  rss = g_new (RSSFeed, 1);
  g_assert (rss != NULL);

  rss->title = g_string_sized_new (20);
  rss->link  = g_string_sized_new (20);
  rss->desc  = g_string_sized_new (20);
  rss->items = NULL;

  return rss;
}

void
rss_feed_free (RSSFeed *rss)
{
  GList *node;

  g_assert (rss != NULL);

  g_string_free (rss->title, TRUE);
  g_string_free (rss->link, TRUE);
  g_string_free (rss->desc, TRUE);

  for (node = rss->items; node != NULL; node = node->next) {
    RSSFeedItem *item = (RSSFeedItem *) node->data;
    g_string_free (item->title, TRUE);
    g_string_free (item->link, TRUE);
    g_string_free (item->desc, TRUE);
    g_free (item);
  }

  g_free (rss);
}

void
rss_feed_clear (RSSFeed *rss)
{
  GList *node;

  g_assert (rss != NULL);

  g_string_free (rss->title, TRUE);
  g_string_free (rss->link, TRUE);
  g_string_free (rss->desc, TRUE);

  for (node = rss->items; node != NULL; node = node->next) {
    RSSFeedItem *item = (RSSFeedItem *) node->data;
    g_string_free (item->title, TRUE);
    g_string_free (item->link, TRUE);
    g_string_free (item->desc, TRUE);
    g_free (item);
  }

  rss->title = g_string_sized_new (20);
  rss->link  = g_string_sized_new (20);
  rss->desc  = g_string_sized_new (20);
  rss->items = NULL;
}

gboolean
rss_feed_parse (RSSFeed     *rss,
                const gchar *text,
                gsize        len)
{
  GMarkupParseContext *context;
  gboolean result;

  g_assert (rss != NULL);

  context = g_markup_parse_context_new (&main_parser, 0, rss, NULL);
  g_assert (context != NULL);

  result = g_markup_parse_context_parse (context, text, len, NULL);
  g_markup_parse_context_free (context);

  return result;
}
