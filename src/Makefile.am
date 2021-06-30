noinst_PROGRAMS 	= osd-example
bin_PROGRAMS 	= osd-demo osd-show-file osd-show-key

osd_demo_SOURCES  = osd-demo.c

osd_demo_LDADD 	= libxosd-xft/libxosd-xft.la @XFT_LIBS@ @X11_LIBS@ @XRENDER_LIBS@

osd_show_file_SOURCES  = osd-show-file.c

osd_show_file_LDADD 	= libxosd-xft/libxosd-xft.la @XFT_LIBS@ @X11_LIBS@ @XRENDER_LIBS@

osd_show_key_SOURCES  = osd-show-key.c utf8.c nerdfonts.c

osd_show_key_LDADD 	= libxosd-xft/libxosd-xft.la @XFT_LIBS@ @X11_LIBS@ @XRENDER_LIBS@

osd_example_SOURCES  = osd-example.c

osd_example_LDADD 	= libxosd-xft/libxosd-xft.la @XFT_LIBS@ @X11_LIBS@ @XRENDER_LIBS@

include_HEADERS = xosd-xft.h

AM_CFLAGS = @XFT_CFLAGS@

SUBDIRS=libxosd-xft
