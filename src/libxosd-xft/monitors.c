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

#include "intern.h"

/* get_focus_point -- get the focus point to select monitor {{{ */
static void get_focus_point(Display *display, int *x_ret, int *y_ret)
{
  int revert_to;
  XWindowAttributes attr;
  Window wid ;

  FUNCTION_START();
  *x_ret = *y_ret = 0;
  XGetInputFocus(display, &wid, &revert_to);
  if (wid != None && XGetWindowAttributes(display, wid, &attr) != 0) {
    int x, y;
    Window unused_child;

    /* The coordinates in attr are relative to the parent window.  If
     * the parent window is the root window, then the coordinates are
     * correct.  If the parent window isn't the root window --- which
     * is likely --- then we translate them. */
    Window parent;
    Window root;
    Window* children;
    unsigned int nchildren;
    XQueryTree(display, wid, &root, &parent, &children, &nchildren);
    if (children != NULL) {
      XFree(children);
    }
    if (parent == attr.root) {
      x = attr.x;
      y = attr.y;
    } else {
      XTranslateCoordinates(display, wid, attr.root,
                            attr.x, attr.y, &x, &y, &unused_child);
    }

    if (x_ret != NULL) {
      *x_ret = x;
    }

    if (y_ret != NULL) {
      *y_ret = y;
    }
  }
  DEBUG_MSG(Dvalue, "FocusPoint { x = %d, y = %d }", *x_ret, *y_ret);
  FUNCTION_END();
}

/* }}} */

/* soxsd_init_monitor -- Initialize multihead if xinerama or xrandr available {{{ */
struct monitor {
  int       number ;
  int       x, y;
  unsigned  width, height;
  int       primary ;
};

static struct monitor*
get_monitors(Display *display, int *n, int use_xrandr, int use_xinerama)
{
  FUNCTION_START();
  struct monitor* r = NULL;
  *n = 0;
  char* used = NULL;
#ifdef HAVE_LIBXRANDR
  if(use_xrandr) {
    XRRMonitorInfo *monitors = XRRGetMonitors(display, DefaultRootWindow(display), True, n);
    if(monitors != NULL) {
      r = calloc(*n, sizeof(struct monitor));
      for(int i = 0; i < *n; i++) {
        r[i].number = i;
        r[i].primary = monitors[i].primary;
        r[i].x = monitors[i].x;
        r[i].y = monitors[i].y;
        r[i].width = monitors[i].width;
        r[i].height = monitors[i].height;
      }
      XRRFreeMonitors(monitors);
#ifdef HAVE_LIBXINERAMA
      use_xinerama = 0;
#endif
      used = "Xrandr";
    }
  }
#endif
#ifdef HAVE_LIBXINERAMA
  if(use_xinerama) {
    int dummy_a, dummy_b;
    XineramaScreenInfo *screeninfo = NULL;
    if (XineramaQueryExtension(display, &dummy_a, &dummy_b) &&
            (screeninfo = XineramaQueryScreens(display, n)) &&
            XineramaIsActive(display)) {
      r = calloc(*n, sizeof(struct monitor));
      for(int i = 0; i < *n; i++) {
        r[i].number = i;
        r[i].primary = 1; /* All monitors are promary */
        r[i].x = screeninfo[i].x_org;
        r[i].y = screeninfo[i].y_org;
        r[i].width = screeninfo[i].width;
        r[i].height = screeninfo[i].height;
      }
    }
    if (screeninfo)
      XFree(screeninfo);
    used = "Xinerama" ;
  }
#endif
  if(r == NULL) {
    DEBUG_MSG(Dvalue, "GetMonitors { }");
  } else {
    DEBUG_MSG(Dvalue, "GetMonitors { driver: %s, monitors: %d }", used, *n);
  }
  FUNCTION_END();
  return r;
}

int osd_init_monitor(Display *display, int screen, int default_monitor, int use_xrandr, int use_xinerama,
                       unsigned int *width, unsigned int *height, int *xpos, int *ypos)
{
  FUNCTION_START();
  int monitor = default_monitor;
  int n;
  int i;
  struct monitor *monitors;
  int focus_x = 0, focus_y = 0;

  *width = XDisplayWidth(display, screen);
  *height = XDisplayHeight(display, screen);
  *xpos = 0;
  *ypos = 0;

  monitors = get_monitors(display, &n, use_xrandr, use_xinerama);
  if(monitors == NULL || n <= 0)
    return 0;

  if(monitor > n && monitor != ACTIVE && monitor != PRIMARY) {
    free(monitors);
    FUNCTION_END();
    fail(-1, "Invalid monitor selection");
  }
  if(monitor == -1 || monitor == ACTIVE) {
    monitor = 0;
    get_focus_point(display, &focus_x, &focus_y);
    for (i = 0; i < n; i++) {
      if (focus_x >= monitors[i].x && focus_x < monitors[i].x + monitors[i].width &&
            focus_y >= monitors[i].y && focus_y < monitors[i].y + monitors[i].height) {
        monitor = i;
        break;
      }
    }
  } else if (monitor == PRIMARY) {
    monitor = 0;
    for (i = 0; i < n; i++) {
      if (monitors[i].primary) {
        monitor = i;
        break;
      }
    }
  }
  *width = monitors[monitor].width;
  *height = monitors[monitor].height;
  *xpos = monitors[monitor].x;
  *ypos = monitors[monitor].y;
  FUNCTION_END();
  return 0;
}

/* {{{
 vim: foldmethod=marker tabstop=2 shiftwidth=2 expandtab
 }}} */
