/*

Copyright 2021 Dakshinamurthy Karra (dakshinamurthy.karra@jaliansystems.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ctype.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

#include <X11/Xft/Xft.h>
#include <X11/Xatom.h>

#ifdef HAVE_LIBXINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#ifdef HAVE_LIBXRANDR
#include <X11/extensions/Xrandr.h>
#endif

#include <xosd-xft.h>

#ifdef DEBUG
/* gcc -O2 optimizes debugging away if Dnone is chosen. {{{ */
typedef enum _DEBUG_LEVEL {
  Dnone = 0,          /* Nothing */
  Dfunction = (1<<0), /* Function enter/exit */
  Dlocking = (1<<1),  /* Locking */
  Dselect = (1<<2),   /* select() processing */
  Dtrace = (1<<3),    /* Programm progess */
  Dvalue = (1<<4),    /* Computed values */
  Dupdate = (1<<5),   /* Display update processing */
  Dall = -1           /* Everything */
} DEBUG_LEVEL ;

extern DEBUG_LEVEL _xosd_debug_level ;
extern int _xosd_debug_fn_level ;

#define DEBUG_MSG(lvl, fmt, ...) \
  do { \
    if (_xosd_debug_level & lvl) { \
      fprintf (stderr, "%10s:%-4d %s: ", __FILE__, __LINE__, \
          __PRETTY_FUNCTION__ ); \
      fprintf (stderr, fmt "\n", ## __VA_ARGS__); \
    } \
  } while (0)
#define FUNCTION_START() \
  do { \
    if (_xosd_debug_level & Dfunction ) \
      fprintf (stderr, "%10s:%-4d %*s>>>%s\n", __FILE__, __LINE__, \
          (_xosd_debug_fn_level++ * 4), "", __PRETTY_FUNCTION__); \
  } while (0)
#define FUNCTION_END() \
  do { \
    if (_xosd_debug_level & Dfunction) \
      fprintf (stderr, "%10s:%-4d %*s<<<%s\n",  __FILE__, __LINE__, \
          (--_xosd_debug_fn_level * 4), "", __PRETTY_FUNCTION__); \
  } while (0)
/* }}} */
#else
#define DEBUG_MSG(lvl, fmt, ...) do{}while(0)
#define FUNCTION_START() do{}while(0)
#define FUNCTION_END() do{}while(0)
#endif

typedef struct _osd_settings
{
  const char*           geometry;
  int                   use_xrandr;
  int                   use_xinerama;
  int                   monitor;
  char**                lines;
  int                   maxlines;
  int                   nlines;
  const char*           fontname;
  const char*           color;
  const char*           bg_color;
  unsigned int          bg_alpha;
  const char*           shadow_color;
  int                   shadow_offset;
  const char*           padding;
  const char*           text_align;
} osd_settings;

struct xosd_xft
{
  /* Thread */
  pthread_t               event_thread;

  /* Display */
  Display*                display;
  Display*                event_display;
  int                     screen;
  Visual*                 visual;
  Colormap                colormap;
  int                     depth;

  /* Screen Geometry */
  unsigned int            screen_width;
  unsigned int            screen_height;
  int                     screen_xpos;
  int                     screen_ypos;

  /* Drawables */
  Window                  window;
  XftDraw*                draw;

  /* Font */
  XftFont*                font;

  /* Colors */
  XftColor                color;
  XftColor                bg_color;
  XftColor                shadow_color;

  /* Parsed Geometry */
  osd_geometry            geometry;

  /* User Settings */
  osd_settings          settings;

  /* Window Calculated Geometry */
  int                     w_x;
  int                     w_y;
  unsigned int            w_border_width;
  unsigned int            w_width;
  unsigned int            w_height;
  unsigned int            line_height;

  /* Text Size - without padding */
  unsigned int            t_width;
  unsigned int            t_height;

  /* Padding */
  unsigned int            w_pad_t;
  unsigned int            w_pad_r;
  unsigned int            w_pad_b;
  unsigned int            w_pad_l;
};

/* Geometry */
int parse_geometry(const char *g,
                   unsigned int *w, unsigned int *h,
                   int *w_per, int *h_per,
                   int *w_chars, int *h_lines,
                   unsigned int *xoff, unsigned int *yoff,
                   int *xneg, int *yneg,
                   osd_valign *valign, osd_halign *halign);

int parse_text_align(const char *align, osd_halign *halign, osd_valign *valign);

/* Monitors */
int osd_init_monitor(Display *display, int screen,
                       int default_monitor,
                       int use_xrandr, int use_xinerama,
                       unsigned int *width, unsigned int *height,
                       int *xpos, int *ypos);

/* Events */
void send_event(xosd_xft *osd, long event_type);

/* Colors */
int init_color(xosd_xft* osd, const char* color, unsigned int alpha, XftColor* xft_color);

#define XOSD_XFT_event                    "XOSD_XFT_EVENT"
#define XOSD_XFT_event_Exit               (1 << 0)
#define XOSD_XFT_event_Hide               (1 << 1)
#define XOSD_XFT_event_Show               (1 << 2)
#define XOSD_XFT_event_Geometry           (1 << 3)
#define XOSD_XFT_event_Foreground         (1 << 4)
#define XOSD_XFT_event_Background         (1 << 5)
#define XOSD_XFT_event_Shadow             (1 << 6)

#define fail(r, m)              \
  do                            \
  {                             \
    if (r == 0)                 \
      return 0;                 \
    osd_error = m;            \
    fprintf(stderr, "%s\n", m); \
    return r;                   \
  } while (0)
