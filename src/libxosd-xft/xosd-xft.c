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

char *osd_default_font = "saucecodepro nerd font:size=64";
char *osd_error;
#ifdef DEBUG
int _xosd_debug_fn_level = 0;
DEBUG_LEVEL _xosd_debug_level = Dnone;
#endif

/* load_defaults -- set sensible defaults for settings {{{ */
void load_defaults(osd_settings *settings)
{
  FUNCTION_START();
  settings->geometry = "0x0+0+0*top/center";
  settings->monitor = -1;
#ifdef HAVE_LIBXRANDR
  settings->use_xrandr = 1;
#endif
#ifdef HAVE_LIBXINERAMA
  settings->use_xinerama = 1;
#endif
  settings->fontname = osd_default_font;
  settings->color = "#0000ee";
  settings->bg_color = "black";
  settings->bg_alpha = 100;
  settings->shadow_color = "lightgrey";
  settings->shadow_offset = 0;
  settings->padding = "0";
  settings->text_align = "center";
  settings->maxlines = 1;
  settings->lines = calloc(1, sizeof(char*));
  FUNCTION_END();
}

/* }}} */

/* osd_create -- Create a new xosd_xft structure {{{ */
xosd_xft *
osd_create()
{
  FUNCTION_START();
  struct xosd_xft *osd = calloc(1, sizeof(struct xosd_xft));
  if (osd == NULL)
  {
    osd_error = "Could not allocate memory...";
    FUNCTION_END();
    return NULL;
  }
  load_defaults(&osd->settings);
  FUNCTION_END();
  return osd;
}

/* }}} */

/* stay_on_top -- Tell window manager to put window topmost. {{{ */
static void
stay_on_top(Display *dpy, Window win)
{
  FUNCTION_START();
  Atom gnome, net_wm, type;
  int format;
  unsigned long nitems, bytesafter;
  unsigned char *args = NULL;
  Window root = DefaultRootWindow(dpy);

  /*
   * build atoms
   */
  gnome = XInternAtom(dpy, "_WIN_SUPPORTING_WM_CHECK", False);
  net_wm = XInternAtom(dpy, "_NET_SUPPORTED", False);

  /*
   * gnome-compilant
   * tested with icewm + WindowMaker
   */
  if (Success == XGetWindowProperty(dpy, root, gnome, 0, (65536 / sizeof(long)), False,
                                    AnyPropertyType, &type, &format, &nitems, &bytesafter, &args) &&
      nitems > 0)
  {
    /*
     * FIXME: check capabilities
     */
    XClientMessageEvent xev;
    Atom gnome_layer = XInternAtom(dpy, "_WIN_LAYER", False);

    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.window = win;
    xev.message_type = gnome_layer;
    xev.format = 32;
    xev.data.l[0] = 6 /* WIN_LAYER_ONTOP */;

    XSendEvent(dpy, DefaultRootWindow(dpy), False, SubstructureNotifyMask,
               (XEvent *)&xev);
    XFree(args);
  }
  /*
   * netwm compliant.
   * tested with kde
   */
  else if (Success == XGetWindowProperty(dpy, root, net_wm, 0, (65536 / sizeof(long)), False,
                                         AnyPropertyType, &type, &format, &nitems, &bytesafter, &args) &&
           nitems > 0)
  {
    XEvent e;
    Atom net_wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom net_wm_top = XInternAtom(dpy, "_NET_WM_STATE_STAYS_ON_TOP", False);

    memset(&e, 0, sizeof(e));
    e.xclient.type = ClientMessage;
    e.xclient.message_type = net_wm_state;
    e.xclient.display = dpy;
    e.xclient.window = win;
    e.xclient.format = 32;
    e.xclient.data.l[0] = 1 /* _NET_WM_STATE_ADD */;
    e.xclient.data.l[1] = net_wm_top;
    e.xclient.data.l[2] = 0l;
    e.xclient.data.l[3] = 0l;
    e.xclient.data.l[4] = 0l;

    XSendEvent(dpy, DefaultRootWindow(dpy), False,
               SubstructureRedirectMask, &e);
    XFree(args);
  }
  /* XMapRaised(dpy, win); */
  FUNCTION_END();
}

/* }}} */

/* osd_parse_geometry -- Parse (user-given) geometry string and create osd_geometry{{{ */
osd_geometry *
osd_parse_geometry(const char *geometry, const char *text_align, osd_geometry *geo)
{
  FUNCTION_START();
  /* Defaults */
  geo->width = 100;
  geo->width_per = 1;
  geo->width_chars = 0;
  geo->height = 0;
  geo->height_per = 0;
  geo->height_lines = 0;
  geo->xoffset = 0;
  geo->xneg = 0;
  geo->yoffset = 0;
  geo->yneg = 0;
  geo->valign = XOSD_XFT_valign_none;
  geo->halign = XOSD_XFT_halign_none;

  if (geometry == NULL) {
    FUNCTION_END();
    return geo;
  }

  if (parse_geometry(geometry, &geo->width, &geo->height,
                     &geo->width_per, &geo->height_per,
                     &geo->width_chars, &geo->height_lines,
                     &geo->xoffset, &geo->yoffset,
                     &geo->xneg, &geo->yneg,
                     &geo->valign, &geo->halign) &&
      parse_text_align(text_align, &geo->text_halign, &geo->text_valign)) {
    FUNCTION_END();
    return geo;
  }
  FUNCTION_END();
  return NULL;
}

/* }}} */

/* init_padding -- Initialize padding values from settings {{{ */
void init_padding(xosd_xft *osd)
{
  FUNCTION_START();
  if (osd->settings.padding != NULL)
  {
    char *padding = strdup(osd->settings.padding);
    char *s = strtok(padding, " ");
    unsigned int l_padding[] = {-1, -1, -1, -1};
    int i = 0;
    do
    {
      l_padding[i++] = atoi(s);
      if (i > 3)
        break;
    } while ((s = strtok(NULL, " ")) != NULL);
    osd->w_pad_t = l_padding[0];
    if (l_padding[1] == -1)
    {
      osd->w_pad_r = l_padding[0];
      osd->w_pad_b = l_padding[0];
      osd->w_pad_l = l_padding[0];
    }
    else if (l_padding[2] == -1)
    {
      osd->w_pad_r = l_padding[1];
      osd->w_pad_b = l_padding[0];
      osd->w_pad_l = l_padding[1];
    }
    else if (l_padding[3] == -1)
    {
      osd->w_pad_r = l_padding[1];
      osd->w_pad_b = l_padding[2];
      osd->w_pad_l = l_padding[1];
    }
    else
    {
      osd->w_pad_r = l_padding[1];
      osd->w_pad_b = l_padding[2];
      osd->w_pad_l = l_padding[3];
    }
    free(padding);
  } else {
    osd->w_pad_t = osd->w_pad_r = osd->w_pad_b = osd->w_pad_l = 0;
  }
  FUNCTION_END();
}

/* }}} */

/* min -- Find minimum of two numbers */
unsigned int
min(unsigned int x, unsigned int y)
{
  return x < y ? x : y;
}

#define DIVCEIL(n, d)		(((n) + ((d) - 1)) / (d))

/* calc_geometry -- Calculate the geometry of OSD Window {{{ */
void calc_geometry(xosd_xft *osd, osd_geometry *geometry)
{
  FUNCTION_START();
  int xneg = geometry->xneg, yneg = geometry->yneg, xoffset = geometry->xoffset, yoffset = geometry->yoffset;
  osd_valign valign = geometry->valign;
  osd_halign halign = geometry->halign;
  int xstart, ystart;
  unsigned int width, height;
  unsigned int line_height, char_width;

  {
    XGlyphInfo extents;
    static char ascii_printable[] =
	      " !\"#$%&'()*+,-./0123456789:;<=>?"
	      "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
	      "`abcdefghijklmnopqrstuvwxyz{|}~";
	  XftTextExtentsUtf8(osd->display, osd->font, (const FcChar8 *) ascii_printable, strlen(ascii_printable), &extents);
	  char_width = DIVCEIL(extents.xOff, strlen(ascii_printable));
  }
  init_padding(osd);

  line_height = osd->font->ascent + osd->font->descent;
  /* char_width = osd->font->max_advance_width; */

  DEBUG_MSG(Dvalue, "CalcGeometry { char_width: %u, width: %u }", char_width, geometry->width);
  width = min(osd->screen_width,
            geometry->width == 0 ?  osd->screen_width :
                (geometry->width_per ? osd->screen_width * geometry->width / 100 :
                  (geometry->width_chars ? geometry->width * char_width + osd->w_pad_l + osd->w_pad_r :
                    geometry->width)));
  height = min(osd->screen_height,
            geometry->height == 0 ? line_height + osd->w_pad_t + osd->w_pad_b :
              (geometry->height_per ? osd->screen_height * geometry->height / 100 :
                (geometry->height_lines ? geometry->height * line_height  + osd->w_pad_t + osd->w_pad_b :
                  geometry->height)));

  osd->t_width = width - osd->w_pad_l - osd->w_pad_r;
  osd->t_height = height - osd->w_pad_t - osd->w_pad_b;

  if(osd->settings.maxlines > osd->t_height / line_height) {
    int maxlines = osd->t_height / line_height ;
    fprintf(stderr, "Number of lines > displayable lines. Adjusting to %d", maxlines);
    osd->settings.maxlines = maxlines ;
  }
  osd->w_width = width;
  osd->w_height = height;
  osd->line_height = line_height;

  DEBUG_MSG(Dvalue, "CalcGeometry { width: %u, height: %u }", osd->w_width, osd->w_height);
  if (xneg)
    xoffset *= -1;
  if (halign == XOSD_XFT_halign_none)
  {
    if (xneg)
    {
      xstart = osd->screen_xpos + osd->screen_width - width;
    }
    else
    {
      xstart = osd->screen_xpos;
    }
  }
  else if (halign == XOSD_XFT_left)
  {
    xstart = osd->screen_xpos;
  }
  else if (halign == XOSD_XFT_right)
  {
    xstart = osd->screen_xpos + osd->screen_width - width;
  }
  else
  {
    xstart = osd->screen_xpos + (osd->screen_width - width) / 2;
  }
  if (yneg)
    yoffset *= -1;
  if (valign == XOSD_XFT_valign_none)
  {
    if (yneg)
    {
      ystart = osd->screen_ypos + osd->screen_height - height;
    }
    else
    {
      ystart = osd->screen_ypos;
    }
  }
  else if (valign == XOSD_XFT_top)
  {
    ystart = osd->screen_ypos;
  }
  else if (valign == XOSD_XFT_bottom)
  {
    ystart = osd->screen_ypos + osd->screen_height - height;
  }
  else
  {
    ystart = osd->screen_ypos + (osd->screen_height - height) / 2;
  }
  osd->w_x = xstart + xoffset;
  osd->w_y = ystart + yoffset;
  osd->w_border_width = 0;
  FUNCTION_END();
}

/* }}} */

/* event_loop -- X11 event loop {{{ */
static void *
event_loop(void *osdv)
{
  xosd_xft *osd = osdv;
  Atom xosd_xft_event =  XInternAtom(osd->display, XOSD_XFT_event, False);

  while (1) {
    XEvent ev;

    XNextEvent(osd->display, &ev);
    if (ev.type == Expose) {
      XftDrawRect(osd->draw, &osd->bg_color, 0, 0, osd->w_width, osd->w_height);
      if (osd->settings.lines != NULL && osd->settings.nlines > 0) {
        XRectangle clip;
        int i;
        clip.x = osd->w_pad_l;
        clip.y = osd->w_pad_t;
        clip.width = osd->w_width - osd->w_pad_r - osd->w_pad_l;
        clip.height = osd->w_height - osd->w_pad_b - osd->w_pad_t;
        XftDrawSetClipRectangles(osd->draw, 0, 0, &clip, 1);
        for(i = 0; i < osd->settings.nlines; i++) {
          char *message = osd->settings.lines[i];
          XGlyphInfo extents;
          XftTextExtentsUtf8(osd->display, osd->font, (XftChar8 *)message, strlen(message), &extents);
          DEBUG_MSG(Dvalue, "Extents { width = %d, height = %d, x = %d, y = %d, xOff = %d, yOff = %d }", extents.width, extents.height, extents.x, extents.y, extents.xOff, extents.yOff);
          DEBUG_MSG(Dvalue, "Geometry: { w_x = %d, w_y = %d, w_border_width = %d, w_width = %d, w_height = %d, t_width = %d, t_height = %d, w_pad_t = %d, w_pad_r = %d, w_pad_b = %d, w_pad_l = %d}", osd->w_x, osd->w_y, osd->w_border_width, osd->w_width, osd->w_height, osd->t_width, osd->t_height, osd->w_pad_t, osd->w_pad_r, osd->w_pad_b, osd->w_pad_l);
          int x = osd->w_pad_l + extents.x;
          int y = osd->w_pad_t + extents.y;
          if(osd->geometry.text_halign == XOSD_XFT_right) {
            x += osd->t_width - extents.width ;
          } else if(osd->geometry.text_halign == XOSD_XFT_center) {
            x += (osd->t_width - extents.width)/2 ;
          }
          if(osd->settings.maxlines <= 1) {
            if(osd->geometry.text_valign == XOSD_XFT_bottom) {
              y += osd->t_height - extents.height;
            } else if(osd->geometry.text_valign == XOSD_XFT_middle) {
              y += (osd->t_height - extents.height)/2 ;
            }
          } else {
            y = osd->line_height * i + osd->w_pad_t + extents.y ;
          }
          if(osd->settings.shadow_offset) {
            XftDrawStringUtf8(osd->draw, &osd->shadow_color, osd->font, x + osd->settings.shadow_offset, y + osd->settings.shadow_offset, (const FcChar8 *)message, strlen(message));
          }
          XftDrawStringUtf8(osd->draw, &osd->color, osd->font, x, y, (const FcChar8 *)message, strlen(message));
        }
        XftDrawSetClip(osd->draw, NULL);
      }
    }
    else if (ev.type == ClientMessage && ev.xclient.message_type == xosd_xft_event) {
      if (ev.xclient.data.l[0] ==  XOSD_XFT_event_Exit) {
        break;
      } else if (ev.xclient.data.l[0] ==  XOSD_XFT_event_Show) {
        XMapRaised(osd->display, osd->window);
      } else if (ev.xclient.data.l[0] ==  XOSD_XFT_event_Hide) {
        XUnmapWindow(osd->display, osd->window);
      } else if (ev.xclient.data.l[0] ==  XOSD_XFT_event_Geometry) {
        calc_geometry(osd, &osd->geometry);
        XMoveResizeWindow(osd->display, osd->window, osd->w_x, osd->w_y, osd->w_width, osd->w_height);
      } else if (ev.xclient.data.l[0] ==  XOSD_XFT_event_Foreground) {
        XftColor xft_color ;
        if(init_color(osd, osd->settings.color, 0, &xft_color) != -1) {
          XftColorFree(osd->display, osd->visual, osd->colormap, &osd->color);
          osd->color = xft_color;
        } else {
          fprintf(stderr, "Error in setting color %s: %s (ignoring)\n", osd->settings.color, osd_error);
        }
      } else if (ev.xclient.data.l[0] ==  XOSD_XFT_event_Background) {
        XftColor xft_color ;
        if(init_color(osd, osd->settings.bg_color, osd->settings.bg_alpha, &xft_color) != -1) {
          XftColorFree(osd->display, osd->visual, osd->colormap, &osd->bg_color);
          osd->bg_color = xft_color;
        } else {
          fprintf(stderr, "Error in setting color %s: %s (ignoring)\n", osd->settings.bg_color, osd_error);
        }
      } else if (ev.xclient.data.l[0] ==  XOSD_XFT_event_Shadow) {
        XftColor xft_color ;
        if(init_color(osd, osd->settings.shadow_color, 0, &xft_color) != -1) {
          XftColorFree(osd->display, osd->visual, osd->colormap, &osd->shadow_color);
          osd->shadow_color = xft_color;
        } else {
          fprintf(stderr, "Error in setting color %s: %s (ignoring)\n", osd->settings.bg_color, osd_error);
        }
      }
    }
  }
  return NULL;
}

/* }}} */

/* init_color -- Initialize a color with alpha channel {{{ */
int
init_color(xosd_xft* osd, const char* color, unsigned int alpha, XftColor* xft_color)
{
  FUNCTION_START();
  XRenderColor bgRender;
  if (!XftColorAllocName(osd->display, osd->visual, osd->colormap, color, xft_color)) {
    osd_error = "Could not allocate color";
    return -1;
  }
  if(alpha < 100) {
    bgRender = xft_color->color;
    bgRender.alpha = alpha / 100.0 * 0xFFFF;
    XftColorFree(osd->display, osd->visual, osd->colormap, xft_color);
    if (!XftColorAllocValue(osd->display, osd->visual, osd->colormap, &bgRender, xft_color)) {
      osd_error ="Could not allocate background color";
      return -1;
    }
  }
  return 0;
}

/* }}} */

/* init_bg_color -- Initialize background color with alpha {{{ */
static int
init_bg_color(xosd_xft* osd, const char* color, unsigned int alpha)
{
  XftColor bg_color;
  if(init_color(osd, color, alpha, &bg_color) != -1) {
    osd->bg_color = bg_color ;
    return 0;
  }
  return -1;
}

/* }}} */

/* osd_init -- Initialize OSD Window {{{ */
static int
osd_init(xosd_xft *osd)
{
  FUNCTION_START();
  XVisualInfo vinfo;
  XSetWindowAttributes winattr;

  /* Initialization. */
  osd->display = XOpenDisplay(NULL);
  if (!osd->display) {
    FUNCTION_END();
    fail(-1, "Could not connect to display");
  }
  osd->event_display = XOpenDisplay(NULL);
  if (!osd->event_display) {
    FUNCTION_END();
    fail(-1, "Could not connect to display");
  }
  osd->screen = DefaultScreen(osd->display);
  if (XMatchVisualInfo(osd->display, osd->screen, 32, TrueColor, &vinfo))
  {
    osd->visual = vinfo.visual;
    osd->colormap = XCreateColormap(osd->display, RootWindow(osd->display, osd->screen), osd->visual, AllocNone);
    osd->depth = vinfo.depth;
  }
  else
  {
    osd->visual = DefaultVisual(osd->display, osd->screen);
    osd->colormap = DefaultColormap(osd->display, osd->screen);
    osd->depth = DefaultDepth(osd->display, osd->screen);
  }

  osd_init_monitor(osd->display, osd->screen, osd->settings.monitor,
                     osd->settings.use_xrandr, osd->settings.use_xinerama,
                     &osd->screen_width, &osd->screen_height,
                     &osd->screen_xpos, &osd->screen_ypos);

  osd->font = XftFontOpenName(osd->display, osd->screen, osd->settings.fontname);
  if (!osd->font) {
    FUNCTION_END();
    fail(-1, "Could not open font");
  }
  DEBUG_MSG(Dvalue, "XftFont { ascent = %d, descent = %d, height = %d, max_advance_width = %d }",
      osd->font->ascent, osd->font->descent, osd->font->height, osd->font->max_advance_width);
  calc_geometry(osd, &osd->geometry);

  winattr.override_redirect = 1;
  winattr.border_pixel = 0;
  winattr.background_pixel = 0;
  winattr.colormap = osd->colormap;
  DEBUG_MSG(Dvalue,"CreateWindow { x: %d, y: %d, width: %u, height: %u }",
            osd->w_x, osd->w_y, osd->w_width, osd->w_height );
  osd->window = XCreateWindow(osd->display,
                              RootWindow(osd->display, osd->screen),
                              osd->w_x, osd->w_y,
                              osd->w_width, osd->w_height,
                              osd->w_border_width,
                              osd->depth,
                              CopyFromParent,
                              osd->visual,
                              CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &winattr);
  {
    Atom window_type = XInternAtom(osd->display, "_NET_WM_WINDOW_TYPE", False);
    long value = XInternAtom(osd->display, "_NET_WM_WINDOW_TYPE_NOTIFICATION", False);
    XChangeProperty(osd->display, osd->window, window_type, XA_ATOM,
                    32, PropModeReplace, (unsigned char *) &value, 1 );
  }

  {
    XClassHint class = { "xosd-xft", "xosd-xft" };
    XSetClassHint(osd->display, osd->window, &class);
  }

  XStoreName(osd->display, osd->window, "XOSD_XFT");
  XSelectInput(osd->display, osd->window, ExposureMask);

  /* Xft. */
  if (init_color(osd, osd->settings.color, 0, &osd->color) == -1) {
    FUNCTION_END();
    return -1;
  }
  if (init_color(osd, osd->settings.shadow_color, 0, &osd->shadow_color)) {
    FUNCTION_END();
    return -1;
  }
  if(init_bg_color(osd, osd->settings.bg_color, osd->settings.bg_alpha) == -1) {
      FUNCTION_END();
      return -1;
  }
  osd->draw = XftDrawCreate(osd->display, osd->window, osd->visual, osd->colormap);

  stay_on_top(osd->display, osd->window);

  pthread_create(&osd->event_thread, NULL, event_loop, osd);
  FUNCTION_END();
  return 0;
}

/* }}} */

/* send_expose_event -- send an expose event */
void send_expose_event(xosd_xft *osd)
{
  FUNCTION_START();
  XEvent exppp;

  memset(&exppp, 0, sizeof(exppp));
  exppp.type = Expose;
  exppp.xexpose.window = osd->window;
  XSendEvent(osd->event_display, osd->window, False, ExposureMask, &exppp);
  XFlush(osd->event_display);
  FUNCTION_END();
}

/* }}} */

/* osd_show --  Show the window {{{ */
int osd_show(xosd_xft *osd)
{
  if(osd->display != NULL)
    send_event(osd, XOSD_XFT_event_Show);
  return 0;
}

/* }}} */

/* osd_hide -- Display a string {{{ */
int osd_hide(xosd_xft *osd)
{
  if(osd->display != NULL)
    send_event(osd, XOSD_XFT_event_Hide);
  return 0;
}

/* }}} */

/* osd_display -- Display a string {{{ */
int osd_display(xosd_xft *osd, char *message, int len)
{
  FUNCTION_START();
  int i;
  char *m = strndup(message, len);
  m[len] = '\0';
  if (osd->display == NULL)
  {
    if (osd_init(osd) != 0) {
      FUNCTION_END();
      fail(-1, osd_error);
    }
  }
  if(osd->settings.nlines < osd->settings.maxlines) {
    osd->settings.lines[osd->settings.nlines++] = m ;
  } else {
    for(i = 1; i < osd->settings.maxlines; i++) {
      osd->settings.lines[i-1] = osd->settings.lines[i];
    }
    osd->settings.lines[osd->settings.nlines-1] = m ;
  }
  send_expose_event(osd);
  FUNCTION_END();
  osd_show(osd);
  return 0;
}

/* }}} */

/* send_event -- send event to App {{{ */
void send_event(xosd_xft *osd, long event_type)
{
  FUNCTION_START();
  XEvent event;
  memset(&event, 0, sizeof(event));
  event.xclient.type = ClientMessage;
  event.xclient.serial = 0;
  event.xclient.send_event = True;
  event.xclient.message_type = XInternAtom(osd->event_display, XOSD_XFT_event, False);
  event.xclient.format = 32;
  event.xclient.window = osd->window;
  event.xclient.data.l[0] = event_type;
  XSendEvent(osd->event_display, osd->window, False, NoEventMask, &event);
  XFlush(osd->event_display);
  FUNCTION_END();
}

/* }}} */

/* osd_destroy -- Free OSD resources {{{ */
int osd_destroy(xosd_xft *osd)
{
  FUNCTION_START();
  if(osd->display == NULL)
    return 0;
  send_event(osd, XOSD_XFT_event_Exit);
  pthread_join(osd->event_thread, NULL);
  XftColorFree(osd->display, osd->visual, osd->colormap, &osd->color);
  XftDrawDestroy(osd->draw);
  XDestroyWindow(osd->display, osd->window);
  XCloseDisplay(osd->display);
  XCloseDisplay(osd->event_display);
  {
    int i ;
    for(i = 0; i < osd->settings.maxlines; i++)
      free(osd->settings.lines[i]);
    free(osd->settings.lines);
  }
  free(osd);
  FUNCTION_END();
  return 0;
}

/* }}} */

/* osd_set_geometry -- set geometry {{{ */
void osd_set_geometry(xosd_xft *osd, const osd_geometry *geometry)
{
  FUNCTION_START();
  osd->geometry = *geometry;
  if(osd->display != NULL) {
    send_event(osd, XOSD_XFT_event_Geometry);
    send_expose_event(osd);
  }
  FUNCTION_END();
}

/* }}} */

/* osd_set_font -- set font {{{ */
void osd_set_font(xosd_xft *osd, const char *font)
{
  FUNCTION_START();
  osd->settings.fontname = font;
  FUNCTION_END();
}

/* }}} */

/* osd_set_monitor -- set monitor {{{ */
void osd_set_monitor(xosd_xft *osd, int monitor)
{
  FUNCTION_START();
  osd->settings.monitor = monitor;
  FUNCTION_END();
}

/* }}} */

/* osd_set_padding -- set padding {{{ */
void osd_set_padding(xosd_xft *osd, const char *padding)
{
  FUNCTION_START();
  osd->settings.padding = padding;
  if(osd->display != NULL) {
    init_padding(osd);
    send_expose_event(osd);
  }
  FUNCTION_END();
}

/* }}} */

/* osd_set_color -- set color {{{ */
void osd_set_color(xosd_xft *osd, const char *color)
{
  FUNCTION_START();
  osd->settings.color = color;
  if(osd->display != NULL) {
    send_event(osd, XOSD_XFT_event_Foreground);
    send_expose_event(osd);
  }
  FUNCTION_END();
}

/* }}} */

/* osd_set_bgcolor -- set background color {{{ */
void osd_set_bgcolor(xosd_xft *osd, const char *bgcolor, unsigned int alpha)
{
  FUNCTION_START();
  osd->settings.bg_color = bgcolor;
  osd->settings.bg_alpha = alpha;
  if(osd->display != NULL) {
    send_event(osd, XOSD_XFT_event_Background);
    send_expose_event(osd);
  }
  FUNCTION_END();
}

/* }}} */

/* osd_set_shadowoffset -- set offset for the shadow (0 - None) {{{ */
void osd_set_shadowoffset(xosd_xft *osd, int offset)
{
  FUNCTION_START();
  osd->settings.shadow_offset = offset;
  FUNCTION_END();
}

/* }}} */

/* osd_set_shadowcolor -- set shadow color {{{ */
void osd_set_shadowcolor(xosd_xft *osd, const char *shadowcolor)
{
  FUNCTION_START();
  osd->settings.shadow_color = shadowcolor;
  if(osd->display != NULL) {
    send_event(osd, XOSD_XFT_event_Shadow);
    send_expose_event(osd);
  }
  FUNCTION_END();
}

/* }}} */

/* osd_set_xinerama -- set xinerama {{{ */
void osd_set_xinerama(xosd_xft *osd, int xinerama)
{
  FUNCTION_START();
  osd->settings.use_xinerama = xinerama;
  FUNCTION_END();
}

/* }}} */

/* osd_set_number_of_lines -- Set number of lines to display {{{ */
void osd_set_number_of_lines(xosd_xft *osd, int nlines)
{
  FUNCTION_START();
  if(osd->settings.lines) {
    int i;
    for(i = 0; i < osd->settings.maxlines; i++)
      free(osd->settings.lines[i]);
    free(osd->settings.lines);
  }
  osd->settings.maxlines = nlines;
  osd->settings.lines = calloc(nlines, sizeof(char*));
  FUNCTION_END();
}

/* }}} */
/* osd_set_xrandr -- set xrandr {{{ */
void osd_set_xrandr(xosd_xft *osd, int xrandr)
{
  FUNCTION_START();
  osd->settings.use_xrandr = xrandr;
  FUNCTION_END();
}

/* osd_set_debug_level -- set the debug level {{{ */
#ifdef DEBUG
static int get_level(char *slevel) {
  if(!strcmp(slevel,"none")) return Dnone;
  if(!strcmp(slevel,"function")) return Dfunction;
  if(!strcmp(slevel,"locking")) return Dlocking;
  if(!strcmp(slevel,"select")) return Dselect;
  if(!strcmp(slevel,"trace")) return Dtrace;
  if(!strcmp(slevel,"value")) return Dvalue;
  if(!strcmp(slevel,"update")) return Dupdate;
  if(!strcmp(slevel,"all")) return Dall;
  DEBUG_MSG(Dvalue, "Unknown debug level %s\n", slevel);
  return 0;
}

void
osd_set_debug_level(char *debug_level) {
  _xosd_debug_level = 0;
  if(debug_level) {
    debug_level = strdup(debug_level);
    char *level = strtok(debug_level, ",");
    do {
      _xosd_debug_level |= get_level(level);
    } while((level = strtok(NULL, ","))  != NULL);
    free(debug_level);
  }
}

#endif
/* }}} */

/* }}} */
/* {{{
 vim: foldmethod=marker tabstop=2 shiftwidth=2 expandtab
 }}} */
