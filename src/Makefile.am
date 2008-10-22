## Process this file with automake to produce a Makefile.in file.

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(XML_CPPFLAGS) \
	$(GTK_CFLAGS) \
	-DPREFIX=\"$(prefix)\" \
	-DDATADIR=\"$(datadir)\"

bin_PROGRAMS = gtk-feed

gtk_feed_SOURCES = \
	main.c \
	about.c \
	common.c \
	common.h \
	rssfeed.c \
	rssfeed.h

gtk_feed_LDADD = \
	$(GTK_LIBS) \
	$(XML_LIBS)
