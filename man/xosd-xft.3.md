% XOSD-XFT(3) | XOSD-XFT Programmers Manual
% Dakshinamurthy Karra
% Jun 28 2021

# NAME

osd\_create, osd\_destroy - create and destroy osd objects
\
osd\_show, osd\_hide, osd\_display - display content
\
osd\_parse\_geometry osd\_set\_geometry - set size, position and offsets
\
osd\_set\_font - set font
\
osd\_set\_color, osd\_set\_bgcolor, osd\_set\_shadowcolor, osd\_set\_shadowoffset - color handling
\
osd\_set\_padding, osd\_set\_number\_of\_lines - padding and display lines for content
\
osd\_set\_monitor, osd\_set\_xinerama osd\_set\_xrandr - settings for multihead

# SYNOPSIS

```
#include <xosd-xft.h>

extern char* osd_error;

xosd_xft *osd_create();
int osd_destroy(xosd_xft *osd);
int osd_show(xosd_xft *osda);
int osd_hide(xosd_xft *osda);
int osd_display(xosd_xft *osd, char *message, int len);
osd_geometry* osd_parse_geometry(const char *geometry, const char* textalign, osd_geometry *g);
void osd_set_geometry(xosd_xft *osd, const osd_geometry *geometry);
void osd_set_font(xosd_xft *osd, const char *font);
void osd_set_color(xosd_xft *osd, const char *color);
void osd_set_bgcolor(xosd_xft *osd, const char *bgcolor, unsigned int alpha);
void osd_set_shadowcolor(xosd_xft *osd, const char *shadowcolor);
void osd_set_shadowoffset(xosd_xft *osd, int offset);
void osd_set_padding(xosd_xft *osd, const char *padding);
void osd_set_number_of_lines(xosd_xft *osd, int nlines);
void osd_set_monitor(xosd_xft *osd, int monitor);
void osd_set_xinerama(xosd_xft *osd, int xinerama);
void osd_set_xrandr(xosd_xft *osd, int xrandr);
```

# DESCRIPTION

The **osd_create()** function creates a `xosd_xft` object that is used in subsequent calls. **osd_create()** does not allocate
any X resources till a subsequent call to **osd_display** is made. **osd_destroy** frees the resources used during the course
of operation.

The **osd_show()** and **osd_hide()** methods are used to display and hide the OSD window after it is shown. These can be used
only after **osd_display()** is used to display the window.

The **osd_display()** methods adds the given line to the content. If the window is not yet displayed, **osd_display()** initializes
the required resources and displays the window. The settings are validated during this method call and **display_window()** returns
`-1` on error and the **osd_error** variable contains the error message.

The **osd_parse_geometry()** is a convenience method to initialize a **osd_geometry** object from a string representation. The
**osd_parse_geometry()** returns NULL if the string is not in proper format.

**Geometry String Format**:

```
      geometry: <size><offset>*<alignment>
      size: <width>x<height>
        The `width` is specified as an integer. It can optionally succeed
        by either `%` or `c`. `%` is used to specify the width as a percentage
        of the screen width. `c` is used to specify the width as number of
        characters

      offset: [+-]<xoffset>[+-]<yoffset>
        The `xoffset` and `yoffset` follows the standard X geometry definition
        when alignment is not provided. When alignment is provided the offsets
        are used to adjust the alignmennt positions 

      alignment: <valign>/<halign>
        The `valign` can be any one of `top`, `middle`, or `bottom`. The `halign` can
        be one of `left`, `center` or `right`.

```

The **osd_geometry** structure holds the parsed geometry information and can be used with a subsequent **osd_set_geometry()** call.

**osd_geometry**:
```
/* Parsed Geometry and Text Alignment */
typedef struct _geometry
{
  /* Size - width_per and height_per set to true if % are used
            width_chars and height_lines set to true if char sizes are used */
  unsigned int          width;            /* Width */
  int                   width_per;        /* Is Width Unit as percentage of screen size */
  int                   width_chars;      /* Is Width Unit as number of characters */
  unsigned int          height;           /* Height */
  int                   height_per;       /* Is Height Unit as pecentage of screen size */
  int                   height_lines;     /* Is Height Unit as number of lines */

  /* Position - offsets are abs values, neg set to true when negative */
  unsigned int          xoffset;          /* X Offset - Absolute value */
  int                   xneg;             /* Is X Offset Negative */
  unsigned int          yoffset;          /* Y Offset - Absolute value */
  int                   yneg;             /* Is Y Offset Negative */

  /* Alignment - Horizontal and Vertical Alignment of OSD Window */
  osd_valign          valign;             /* Vertical Alignment */
  osd_halign          halign;             /* Horizontal Alignment */

  /* Alignment - for Text */
  osd_valign          text_valign;        /* Text Vertical Alignment */
  osd_halign          text_halign;        /* Text Horizontal Alignment */
} osd_geometry;

```

**Horizontal and Vertical Alignment Constants**

```
/* Vertical Alignment */
typedef enum
{
  XOSD_XFT_top = 0,       /* Align to Top */
  XOSD_XFT_bottom,        /* Align to Bottom */
  XOSD_XFT_middle,        /* Align to Middle */
  XOSD_XFT_valign_none    /* Unspecified - defaults to Top */
} osd_valign;

/* Horizontal Alignment */
typedef enum
{
  XOSD_XFT_left = 0,      /* Align to Left */
  XOSD_XFT_center,        /* Align to Center */
  XOSD_XFT_right,         /* Align to Righht */
  XOSD_XFT_halign_none    /* Unspecified - defaults to Left */
} osd_halign;

```
**osd_set_geometry()** can be used to modify the geometry. If the window is already displayed, **osd_set_geometry()** 
sets the modifies the displayed window.

The **osd_set_font()** method is used to specify the font used for displaying the content. Setting font after the window is
displayed doesn't have any effect. The **font** parameter is the name of the font in *Xft* format.

The **osd_set_color()**, **osd_set_bgcolor()** and **osd_set_shadowcolor()** methods are used to set the corresponding color values.
Either X11 color names or values can be used for the color parameters. The **alpha** parameter is an integer between 0-100 and sets
the transparency (0 being fully transparent, 100 opaque).

**osd_set_padding** is used to set the padding for the content. The **padding** parameter is a string and uses the CSS convention.
The **osd_set_number_of_lines** method can be used to set the number of lines to display.

The library supports multihead displays using *Xrandr* or *Xinerama* extensions. **osd_set_monitor()** allows you to select a monitor
to display the content. You can set **monitor** to *ACTIVE* or *PRIMARY* to select either active or primary monitor. Active monitor is
the default. **osd_set_xinerama** or **osd_set_xrandr** are used to disable calls to either library.

# EXAMPLES

The following program displays the message on the active monitor.

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <xosd-xft.h>

xosd_xft *osd;

int main(int argc, char *argv[])
{
  osd_geometry g;

  xosd_xft *osd = osd_create();
  if(osd_parse_geometry("300x300+0+0*middle/center", "center/middle", &g) == NULL) {
    fprintf(stderr, "%s\n", osd_error);
    return EXIT_FAILURE;
  }
  osd_set_geometry(osd, &g);
  osd_set_font(osd, "mono:size=16");
  osd_set_color(osd, "lightblue");
  osd_set_bgcolor(osd, "black", 100);

  char* message = "HELLO XOSD-XFT" ;
  osd_display(osd, message, strlen(message));

  usleep(1000000);
  osd_destroy(osd);
  return EXIT_SUCCESS;
}
```
