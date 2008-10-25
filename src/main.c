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

#include <gtk/gtk.h>
#include "common.h"

/* Program main function. */
int
main (int argc, char **argv)
{
  /* Initialize GTK and other libraries. */
  g_thread_init (NULL);
  gdk_threads_init ();
  gtk_init (&argc, &argv);

  /* Initialize the application. */
  g_set_application_name ("GTK Feed Reader");
  gtk_window_set_default_icon_name ("gtk-feed");
  get_feeds_menu ();
  get_status_icon ();

  /* Run the main loop. */
  gtk_main ();

  return 0;
}
