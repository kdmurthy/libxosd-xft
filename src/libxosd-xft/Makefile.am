AM_CFLAGS = -I$(top_srcdir)/src @XFT_CFLAGS@

# Library
lib_LTLIBRARIES 	= libxosd-xft.la
libxosd_xft_la_SOURCES 	= xosd-xft.c geometry.c monitors.c intern.h
libxosd_xft_la_LIBADD 	= $(X_LIBS)
libxosd_xft_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -pthread

 
