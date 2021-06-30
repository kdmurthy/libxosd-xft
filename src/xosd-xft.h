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

#ifndef XOSD_XFT_H
#define XOSD_XFT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <limits.h>

#ifndef __deprecated
#if (__GNUC__ == 3 && __GNUC_MINOR__ > 0) || __GNUC__ > 3
#define __deprecated __attribute__((deprecated))
#else
#define __deprecated
#endif
#endif

extern char *osd_error;
extern char *osd_default_font;
typedef struct xosd_xft xosd_xft;

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

#define PRIMARY (INT_MAX)
#define ACTIVE (INT_MAX - 1)

/* osd_create -- Create a new xosd_xft "object"
*
* RETURNS
*     A new xosd structure.
*/
xosd_xft *osd_create();

/* osd_parse_geometry -- Parse geometry string
*
* ARGUMENTS
*    geometry  The geometry string
*    textalign Text alignment string
*    g         Geometry object to fill       
* RETURNS
*    NULL on error
*/
osd_geometry* osd_parse_geometry(const char *geometry, const char* textalign, osd_geometry *g);

/* osd_set_geometry -- Set the geometry for the OSD window
*
* ARGUMENTS
*    osd       A xosd_xft object
*    geometry  The geometry
*
*/
void osd_set_geometry(xosd_xft *osd, const osd_geometry *geometry);

/* osd_set_font -- Set the font for the OSD window
*
* ARGUMENTS
*    osd       A xosd_xft object
*    font      The font
*
*/
void osd_set_font(xosd_xft *osd, const char *font);

/* osd_set_monitor -- Set the monitor for the OSD window
*
* ARGUMENTS
*    osd       A xosd_xft object
*    monitor   The monitor (starts with: 1)
*
*/
void osd_set_monitor(xosd_xft *osd, int monitor);

/* osd_set_padding -- Set the padding for the OSD window
*
* ARGUMENTS
*    osd       A xosd_xft object
*    padding   The padding
*
*/
void osd_set_padding(xosd_xft *osd, const char *padding);

/* osd_set_color -- Set the color for the OSD window
*
* ARGUMENTS
*    osd       A xosd_xft object
*    color   The color
*
*/
void osd_set_color(xosd_xft *osd, const char *color);

/* osd_set_bgcolor -- Set the bgcolor and alpha for the OSD window
*
* ARGUMENTS
*    osd       A xosd_xft object
*    bgcolor   The background color
*    alpha     Tha alpha value (0-100)
*
*/
void osd_set_bgcolor(xosd_xft *osd, const char *bgcolor, unsigned int alpha);

/* osd_set_shadowcolor -- Set the shadowcolor for the OSD window
*
* ARGUMENTS
*    osd           A xosd_xft object
*    shadowcolor   The shadow color
*
*/
void osd_set_shadowcolor(xosd_xft *osd, const char *shadowcolor);

/* osd_set_shadowoffset -- Set the offset for the shadow (0 - none)
*
* ARGUMENTS
*    osd       A xosd_xft object
*    offset    The offset for the shadow (0 - none)
*
*/
void osd_set_shadowoffset(xosd_xft *osd, int offset);

/* osd_set_xinerama -- Enable/disable xinerama
*
* ARGUMENTS
*    osd       A xosd_xft object
*    xinerama  The xinerama - bool 0 for false
*
*/
void osd_set_xinerama(xosd_xft *osd, int xinerama);

/* osd_set_xrandr -- Enable/disable xrandr
*
* ARGUMENTS
*    osd       A xosd_xft object
*    xrandr  The xrandr - bool 0 for false
*
*/
void osd_set_xrandr(xosd_xft *osd, int xrandr);

/* osd_set_number_of_lines -- Sets the number of lines in display
*
* ARGUMENTS
*    osd       A xosd_xft object
*    nlines    Number of lines
*
*/
void osd_set_number_of_lines(xosd_xft *osd, int nlines);

/* osd_show -- Show OSD Window (previously hidden)
*
* ARGUMENTS
*    osd       A xosd_xft object
*
* RETURNS
*     -1 on failure
*/
int osd_show(xosd_xft *osda);

/* osd_hide -- Hide OSD window
*
* ARGUMENTS
*    osd       A xosd_xft object
*
* RETURNS
*     -1 on failure
*/
int osd_hide(xosd_xft *osda);

/* osd_display -- Display a string in the OSD window
*
* ARGUMENTS
*    osd       A xosd_xft object
*    message   The string to display
*    len       The length of string
*
* RETURNS
*     -1 on failure
*/
int osd_display(xosd_xft *osd, char *message, int len);

/* osd_destroy -- Free all held resources of OSD window
*
* ARGUMENTS
*    osd       A xosd_xft object
*
* RETURNS
*     -1 on failure
*/
int osd_destroy(xosd_xft *osd);

#ifdef DEBUG
/* osd_set_debug_level -- Sets the debug level for the library
*
* ARGUMENTS
*     levels      comma separated levels: none,function,locking,select,trace,value,update,all
*
* RETURNS
*   void
*/
void osd_set_debug_level(char * levels);
#endif

#ifdef __cplusplus
};
#endif

#endif

/* vim: foldmethod=marker tabstop=2 shiftwidth=2 expandtab
*/
