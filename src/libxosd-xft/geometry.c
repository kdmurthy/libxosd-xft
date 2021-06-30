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

/** Names for alignment */
static char *osd_valign_names[] = {"top", "bottom", "middle", "none"};
static char *osd_halign_names[] = {"left", "center", "right", "none"};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/* read_size -- read size part of geometry {{{ */
static const char *
read_size(const char *s, unsigned int *sz, int *per, int *lines_or_chars)
{
  FUNCTION_START();
  *sz = 0;
  *per = 0;
  *lines_or_chars = 0;
  while (isdigit(*s))
  {
    *sz = (*sz * 10) + (*s - '0');
    s++;
  }
  if (*s == '%')
  {
    *per = 1;
    s++;
  }
  else if (*s == 'c' || *s == 'C' || *s == 'l' || *s == 'L') {
    *lines_or_chars = 1;
    s++;
  }
  FUNCTION_END();
  return s;
}

/* }}} */

/* parse_size -- parse width and height from geometry {{{ */
static const char *
parse_size(const char *g, unsigned int *w, unsigned int *h, int *w_per, int *h_per, int *w_chars, int *h_lines)
{
  FUNCTION_START();
  g = read_size(g, w, w_per, w_chars);
  if (*g == 'x' || *g == 'X')
  {
    g = read_size(++g, h, h_per, h_lines);
  }
  FUNCTION_END();
  return g;
}

/* }}} */

/* read_off -- read offset from geometry {{{ */
static const char *
read_off(const char *s, unsigned int *off, int *neg)
{
  FUNCTION_START();
  *neg = *s++ == '-';
  *off = 0;
  while (isdigit(*s))
  {
    *off = (*off * 10) + (*s - '0');
    s++;
  }
  FUNCTION_END();
  return s;
}

/* }}} */

/* parse_off -- parse x and y offsets from geometry {{{ */
static const char *
parse_off(const char *g, unsigned int *xoff, unsigned int *yoff, int *xneg, int *yneg)
{
  FUNCTION_START();
  g = read_off(g, xoff, xneg);
  if (*g == '+' || *g == '-')
    g = read_off(g, yoff, yneg);
  FUNCTION_END();
  return g;
}

/* }}} */

/* parse_pos -- parse hoarizontal and vertial alignments from geometry {{{ */
static const char *
parse_pos(const char *s, osd_valign *valign, osd_halign *halign)
{
  FUNCTION_START();
  int i;
  for (i = 0; i < ARRAY_SIZE(osd_valign_names); i++)
  {
    if (!strncmp(osd_valign_names[i], s, strlen(osd_valign_names[i])))
    {
      *valign = (osd_valign)i;
      s += strlen(osd_valign_names[i]);
      break;
    }
  }
  if (i == ARRAY_SIZE(osd_valign_names))
  {
    osd_error = "Invalid valign. Should be one of top,bottom or middile";
    FUNCTION_END();
    return NULL;
  }
  if (*s != '/') {
    FUNCTION_END();
    return s;
  }
  s++;
  for (i = 0; i < ARRAY_SIZE(osd_halign_names); i++)
  {
    if (!strncmp(osd_halign_names[i], s, strlen(osd_halign_names[i])))
    {
      *halign = (osd_halign)i;
      s += strlen(osd_halign_names[i]);
      break;
    }
  }
  if (i == ARRAY_SIZE(osd_halign_names))
  {
    osd_error = "Invalid halign. Should be one of left,right or center";
    FUNCTION_END();
    return NULL;
  }
  FUNCTION_END();
  return s;
}

/* }}} */

/* parse_geometry -- parse the geometry string {{{ */
int parse_geometry(const char *g,
                   unsigned int *w, unsigned int *h,
                   int *w_per, int *h_per,
                   int *w_chars, int *h_lines,
                   unsigned int *xoff, unsigned int *yoff,
                   int *xneg, int *yneg,
                   osd_valign *valign, osd_halign *halign)
{
  FUNCTION_START();
  osd_error = NULL;
  if (isdigit(*g))
  {
    g = parse_size(g, w, h, w_per, h_per, w_chars, h_lines);
  }
  if (g && (*g == '+' || *g == '-'))
  {
    g = parse_off(g, xoff, yoff, xneg, yneg);
  }
  if (g && (*g == '*'))
  {
    g = parse_pos(++g, valign, halign);
  }
  if (!g || *g != '\0')
  {
    if (osd_error == NULL)
      osd_error = "Invalid geometry string";
    FUNCTION_END();
    return 0;
  }
  FUNCTION_END();
  return 1;
}

/* }}} */

/* parse_text_align -- Parse the alignment string */
int
parse_text_align(const char *align, osd_halign *halign, osd_valign *valign)
{
  FUNCTION_START();
  int i;
  for (i = 0; i < ARRAY_SIZE(osd_halign_names); i++)
  {
    if (!strncmp(osd_halign_names[i], align, strlen(osd_halign_names[i])))
    {
      *halign = (osd_halign)i;
      align += strlen(osd_halign_names[i]);
      break;
    }
  }
  if (i == ARRAY_SIZE(osd_halign_names))
  {
    osd_error = "Invalid halign. Should be one of left,right or center or none";
    FUNCTION_END();
    return 0;
  }
  if (*align != '/') {
    if(*align) {
      osd_error = "Invalid alignment.";
      FUNCTION_END();
      return 0;
    }
    FUNCTION_END();
    return 1;
  }
  align++;
  for (i = 0; i < ARRAY_SIZE(osd_valign_names); i++)
  {
    if (!strncmp(osd_valign_names[i], align, strlen(osd_valign_names[i])))
    {
      *valign = (osd_valign)i;
      align += strlen(osd_valign_names[i]);
      break;
    }
  }
  if (i == ARRAY_SIZE(osd_valign_names))
  {
    osd_error = "Invalid valign. Should be one of top,bottom or middile";
    FUNCTION_END();
    return 0;
  }
  if(*align) {
    osd_error = "Invalid alignment.";
    FUNCTION_END();
    return 0;
  }
  FUNCTION_END();
  return 1;
}

/* {{{
 vim: foldmethod=marker tabstop=2 shiftwidth=2 expandtab
 }}} */
