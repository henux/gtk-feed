## Process this file with automake to produce a Makefile.in file.

pixmapdir = $(pkgdatadir)/pixmaps

bin_PROGRAMS = gtk-feed
gtk_feed_SOURCES = main.c common.c rssfeed.c
gtk_feed_CPPFLAGS = $(XML_CPPFLAGS)
gtk_feed_CFLAGS = $(GTK_CFLAGS) -DPIXMAPDIR=\"$(pixmapdir)\"
gtk_feed_LDADD = $(GTK_LIBS) $(XML_LIBS)
