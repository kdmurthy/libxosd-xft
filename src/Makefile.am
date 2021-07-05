noinst_PROGRAMS 	= osd-example
bin_PROGRAMS 	= osd-demo osd-cat osd-echo

osd_demo_SOURCES  = osd-demo.c

osd_demo_LDADD 	= libxosd-xft/libxosd-xft.la @XFT_LIBS@ @X11_LIBS@ @XRENDER_LIBS@

osd_cat_SOURCES  = osd-cat.c

osd_cat_LDADD 	= libxosd-xft/libxosd-xft.la @XFT_LIBS@ @X11_LIBS@ @XRENDER_LIBS@

osd_echo_SOURCES  = osd-echo.c utf8.c nerdfonts.c

osd_echo_LDADD 	= libxosd-xft/libxosd-xft.la @XFT_LIBS@ @X11_LIBS@ @XRENDER_LIBS@

osd_example_SOURCES  = osd-example.c

osd_example_LDADD 	= libxosd-xft/libxosd-xft.la @XFT_LIBS@ @X11_LIBS@ @XRENDER_LIBS@

include_HEADERS = xosd-xft.h

AM_CFLAGS = @XFT_CFLAGS@

SUBDIRS=libxosd-xft
