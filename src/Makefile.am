## Process this file with automake to produce a Makefile.in file.

# Copyright (C) 2008 Henri Häkkinen.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

bin_PROGRAMS = gtk-feed

gtk_feed_SOURCES = \
	callbacks.c \
	callbacks.h \
	common.c \
	common.h \
	dialogs.c \
	dialogs.h \
	main.c \
	options.c \
	options.h \
	rssfeed.c \
	rssfeed.h

gtk_feed_CPPFLAGS = \
	$(XML_CPPFLAGS) \
	-DPREFIX=\"$(prefix)\" \
	-DG_LOG_DOMAIN=\"GTK-Feed\"

gtk_feed_CFLAGS = \
	-Wall \
	-Werror \
	$(GTK_CFLAGS)

gtk_feed_LDADD = \
	$(GTK_LIBS) \
	$(XML_LIBS)
