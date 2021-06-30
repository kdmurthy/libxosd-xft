dnl
dnl Configure.ac for xosd_xft
dnl

dnl Process this file with autoconf to produce a configure script.
AC_INIT(xosd_xft, 1.0.0)
AM_INIT_AUTOMAKE([no-define])

dnl
dnl Require autoconf version 2.57
dnl
AC_PREREQ(2.57)

dnl The libtool version numbers (LT_*); Don't even think about faking this!
dnl
dnl immediately before every release do:
dnl ===================================
dnl if (the interface is totally unchanged from previous release)
dnl    LT_REVISION ++;
dnl else { /* interfaces have been added, removed or changed */
dnl    LT_REVISION = 0;
dnl    LT_CURRENT ++;
dnl    if (any interfaces have been _added_ since last release)
dnl       AGE ++;
dnl    if (any interfaces have been _removed_ or _incompatibly changed_)
dnl       AGE = 0;
dnl }
LT_CURRENT=4
LT_AGE=2
LT_REVISION=14

LT_INIT

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_AGE)
AC_SUBST(LT_REVISION)

AM_MAINTAINER_MODE

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PATH_XTRA([X11],[X11/Xlib.h],[XrmInitialize()])
# We need two libtools, one that builds both shared and static, and
# one that builds only static.  This is because the resulting libtool
# does not allow us to choose which to build at runtime.
sed -e 's/^build_old_libs=yes/build_old_libs=no/' libtool > libtool-disable-static
chmod +x libtool-disable-static

LIBS="$LIBS"
CFLAGS="$CFLAGS -Wall"
M4DATADIR="$datadir/aclocal"
AC_SUBST(M4DATADIR)

dnl Checks for libraries.
AC_CHECK_LIB(pthread, pthread_create,,
       AC_MSG_ERROR([*** POSIX thread support not found ***]))

dnl Check for header files.
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADER(pthread.h,,
    AC_MSG_ERROR([*** POSIX thread support not installed ***]))

dnl Check for Xft
PKG_CHECK_MODULES([XFT], [xft])
PKG_CHECK_MODULES([X11], [x11])
PKG_CHECK_MODULES([XRENDER], [xrender])

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_FUNC_VPRINTF

# nicked from the irmix project...
AC_ARG_ENABLE([xinerama],
              AS_HELP_STRING([--disable-xinerama],
           [disable use of Xinerama extension]),
              [disable_xinerama="yes"],
        [disable_xinerama="no"])

if test x$disable_xinerama = "xno"
then
  AC_CHECK_LIB(Xinerama,
               XineramaQueryExtension)
fi

# nicked from above and enabling xrandr
AC_ARG_ENABLE([xrandr],
              AS_HELP_STRING([--disable-xrandr],
           [disable use of Xrandr extension]),
              [disable_xrandr="yes"],
        [disable_xrandr="no"])

if test x$disable_xrandr = "xno"
then
  AC_CHECK_LIB(Xrandr,
               XRRQueryExtension)
fi

dnl Define XOSD_XFT_VERSION
AC_DEFINE_UNQUOTED(XOSD_XFT_VERSION, "${VERSION}")

dnl Produce Output files
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_FILES([
xosd-xft.pc
Makefile
man/Makefile
src/Makefile
src/libxosd-xft/Makefile
])
AC_OUTPUT
