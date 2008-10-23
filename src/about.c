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

#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkaboutdialog.h>

#include "common.h"

static gchar *authors[] = {
  "Henri Häkkinen <henux@users.sourcefore.net>", NULL
};

static gchar *license =
  "This program is free software: you can redistribute it and/or modify" \
  "it under the terms of the GNU General Public License as published by" \
  "the Free Software Foundation, either version 3 of the License, or"   \
  "(at your option) any later version.\n\n"                             \
  "This program is distributed in the hope that it will be useful,"     \
  "but WITHOUT ANY WARRANTY; without even the implied warranty of"      \
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"       \
  "GNU General Public License for more details.\n\n"                    \
  "You should have received a copy of the GNU General Public License"   \
  "along with this program.  If not, see <http://www.gnu.org/licenses/>.";

void
show_about_box ()
{
  gtk_show_about_dialog (NULL,
                         "authors", authors,
                         "comments", "A lightweight GTK+ 2.0 RSS feed reader.",
                         "copyright", "Copyright (C) 2008 Henri Häkkinen",
                         "license", license,
                         "logo", icon_48x48,
                         "version", PACKAGE_VERSION,
                         "wrap-license", TRUE,
                         "website", "http://henux.nor.fi/coding/projects/15-gtk/24-gtk-feed",
                         NULL);
}
