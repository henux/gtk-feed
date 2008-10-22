## Process this file with automake to produce a Makefile.in file.

AM_CPPFLAGS = \
	$(XML_CPPFLAGS) \
	$(GTK_CFLAGS) \
	-DPREFIX=\"$(prefix)\" \
	-DDATADIR=\"$(datadir)\"

bin_PROGRAMS = gtk-feed

gtk_feed_SOURCES = \
	main.c \
	about.c \
	common.c \
	rssfeed.c

gtk_feed_LDADD = \
	$(GTK_LIBS) \
	$(XML_LIBS)
