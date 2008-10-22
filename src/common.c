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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include "common.h"

/* Returns a filename to the pixmaps directory.  The caller is responsible
   of free'ing the returned string.  */
gchar *
get_pixmap_filename (const gchar *filename)
{
  return g_build_filename (PIXMAPDIR, filename, NULL);
}

/* Returns a filename to the user's config directory.  The caller is
   responsible of free'ing the returned string. */
gchar *
get_config_filename (const gchar *filename)
{
  return g_build_filename (g_get_user_config_dir (),
                           PACKAGE_NAME, filename, NULL);
}
