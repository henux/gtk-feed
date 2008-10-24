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

#include <glib/gmem.h>
#include <glib/gmessages.h>
#include <glib/gstrfuncs.h>
#include <glib/goption.h>
#include <glib/gprintf.h>

#include <gtk/gtkmain.h>

#include "options.h"

struct options_s options = {
  NULL
};

static GOptionEntry entries[] = {
  {"feeds", 'f', 0, G_OPTION_ARG_FILENAME, &options.feeds_file, "Load feed URLs from FEEDS.", "FEEDS"},
  {0}
};

gboolean
get_options (int *argc, char ***argv)
{
  gchar          *parameter_string;
  gchar          *description;
  GOptionContext *context;
  GError         *error = NULL;
  gboolean        result;

  /* Initialize the option parser. */
  parameter_string = g_strdup_printf ("- %s", g_get_application_name ());
  description = g_strdup_printf ("Report bugs to <%s>.", PACKAGE_BUGREPORT);

  context = g_option_context_new (parameter_string);

  g_option_context_set_description (context, description);
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));

  /* Parse options. */
  result = g_option_context_parse (context, argc, argv, &error);
  if (result == FALSE) {
    g_print ("%s: %s\n", PACKAGE, error->message);
    g_error_free (error);
  } else {
    g_debug ("Command line options:");
    g_debug ("* feeds_file = '%s'", options.feeds_file);
  }

  /* Clean-up. */
  g_option_context_free (context);

  g_free (parameter_string);
  g_free (description);

  return result;
}
