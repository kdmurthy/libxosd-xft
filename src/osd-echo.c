/*

Copyright 2021 Dakshinamurthy Karra (dakshinamurthy.karra@jaliansystems.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xosd-xft.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <sys/time.h>

#ifdef HAVE_LIBXINERAMA
int use_xinerama = True;
#endif

#ifdef HAVE_LIBXRANDR
int use_xrandr = True;
#endif

#define check_error(x, f, p)                        \
  do                                                \
  {                                                 \
    if (x(f, p) == -1)                              \
    {                                               \
      fprintf(stderr, "%s: %s\n", #x, osd_error); \
      return EXIT_FAILURE;                          \
    }                                               \
  } while (0)

static struct option long_options[] = {
    /* Main options */
    {"bg-alpha",        1, NULL, 'a'},
    {"bg-color",        1, NULL, 'b'},
    {"color",           1, NULL, 'c'},
    {"delay-in-millis", 1, NULL, 'd'},
#ifdef DEBUG
    {"debug",           1, NULL, 'D'},
#endif
    {"exec",            1, NULL, 'e'},
    {"font",            1, NULL, 'f'},
    {"geometry",        1, NULL, 'g'},
    {"help",            0, NULL, 'h'},
    {"list-fonts",      2, NULL, 'l'},
#if defined(HAVE_LIBXINERAMA) || defined(HAVE_LIBXRANDR)
    {"monitor",         1, NULL, 'm'},
#endif
    {"shadow-color",    1, NULL, 's'},
    {"shadow-offset",   1, NULL, 'S'},
    {"padding",         1, NULL, 'p'},
    {"text-align",      1, NULL, 't'},

/* Multihead support */
#ifdef HAVE_LIBXINERAMA
    {"no-xinerama",     0, &use_xinerama, False},
#endif
#ifdef HAVE_LIBXRANDR
    {"no-xrandr",       0, &use_xrandr, False},
#endif
    {NULL,              0, NULL, 0}};

xosd_xft *osd;

/* Default Values */
char*     font          = "SauceCodePro Nerd Font:size=64:antialias=true";
char*     color         = "ghostwhite";
char*     bg_color      = "black";
int       bg_alpha      = 50;
char*     shadow_color  = "lightgrey";
int       shadow_offset = 0;
char*     padding       = "0";
int       delay_millis  = 1000;
char*     command       = NULL;

#if defined(HAVE_LIBXINERAMA) || defined(HAVE_LIBXRANDR)
int       monitor = -1;
#endif
char*     geometry = "180x180+0-30*bottom/center";
char*     text_align = "center/middle";
char*     search;
#ifdef DEBUG
char*     debug_level = NULL;
#endif

char *print_utf8(const char *s);
void list_fonts(char *search);
char* find_code(char *);

#define MSG_LEN 2049
static void help(char **argv);

static char*
replace_font_names(char *m)
{
  char buffer[MSG_LEN], *dest = &buffer[0];
  char *src = strdup(m), *to_free = src;
  while(src && *src) {
    if(*src != ':' || *++src == ':') {
      *dest++ = *src++ ;
    } else {
      char *end = strstr(src, ":");
      char *code;
      if(end != NULL)
        *end++ = 0;
      if((code = find_code(src)) != NULL) {
        while(*code)
          *dest++ = *code++;
      } else {
        fprintf(stderr, "Unable to find code for %s\n", src);
      }
      src = end;
    }
  }
  *dest = '\0';
  free(to_free);
  return strdup(buffer);
}

int main(int argc, char *argv[])
{
  osd_geometry g, *parsed = &g;
  if (setlocale(LC_ALL, "") == NULL || !XSupportsLocale())
    fprintf(stderr, "Locale not available, expect problems with fonts.\n");

  while (1)
  {
    int option_index = 0;
    int c =
        getopt_long(argc, argv, "D:f:c:e:l::m:g:p:b:a:d:ht:s:S:",
                    long_options,
                    &option_index);
    if (c == -1)
      break;
    switch (c)
    {
    case 0:
      break;
    case 'f':
      font = optarg;
      break;
    case 'c':
      color = optarg;
      break;
    case 'p':
      padding = optarg;
      break;
    case 'b':
      bg_color = optarg;
      break;
    case 'a':
      bg_alpha = atoi(optarg);
      break;
    case 's':
      shadow_color = optarg;
      break;
    case 'S':
      shadow_offset = atoi(optarg);
      break;
    case 'd':
      delay_millis = atoi(optarg);
      break;
    case 'e':
      command = optarg;
      break;
    case 'g':
      geometry = optarg;
      break;
    case 't':
      text_align = optarg;
      break;
#ifdef DEBUG
    case 'D':
      debug_level = optarg;
      break;
#endif

#if defined(HAVE_LIBXINERAMA) || defined(HAVE_LIBXRANDR)
    case 'm':
#ifdef HAVE_LIBXRANDR
      if (!strcmp(optarg, "active"))
      {
        monitor = ACTIVE;
      }
      else if (!strcmp(optarg, "primary"))
      {
        monitor = PRIMARY;
      }
      else
      {
#endif
        monitor = atoi(optarg);
        if (monitor == 0)
        {
          fprintf(stderr, "Invalid monitor %s. Numbering  starts from 1\n", optarg);
          return EXIT_FAILURE;
        }
        monitor -= 1;
#ifdef HAVE_LIBXRANDR
      }
#endif
      break;
#endif
    case 'l':
      list_fonts(optarg ? optarg : NULL);
      return EXIT_SUCCESS;
    case '?':
    case 'h':
    default:
      help(argv);
      return EXIT_SUCCESS;
    }
  }

  xosd_xft *osd = osd_create();
#ifdef DEBUG
  osd_set_debug_level(debug_level);
#endif
  parsed = osd_parse_geometry(geometry, text_align, parsed);
  if(parsed == NULL) {
    fprintf(stderr, "%s\n", osd_error);
    return EXIT_FAILURE;
  }
  osd_set_geometry(osd, parsed);
  osd_set_font(osd, font);
  osd_set_monitor(osd, monitor);
  osd_set_padding(osd, padding);
  osd_set_color(osd, color);
  osd_set_bgcolor(osd, bg_color, bg_alpha);
  osd_set_shadowcolor(osd, shadow_color);
  osd_set_shadowoffset(osd, shadow_offset);
  osd_set_xinerama(osd, use_xinerama);
  osd_set_xrandr(osd, use_xrandr);

  if(command)
    system(command);

  char *message1 = replace_font_names(optind < argc ? argv[optind] : "XOSD_XFT YaY!!!");
  char *message = print_utf8(message1);
  osd_display(osd, message, strlen(message));

  usleep(delay_millis * 1000);
  osd_destroy(osd);
  free(message);
  free(message1);
  return EXIT_SUCCESS;
}

static void
help(char **argv)
{
      fprintf(stderr, "Usage: %s [OPTION] message...\n", argv[0]);
      fprintf(stderr, "Version: %s\n", XOSD_XFT_VERSION);
      fprintf(stderr,
              "Display a key glyph OSD using nerd fonts\n"
              "The message can contain a font name between ':'. eg: :fa-home:\n"
              "Use --list-fonts to find a known glyph name\n"
              "\n"
              "  -h, --help                  Show this help\n"
              "  -g, --geometry=<geo>        Geometry for the window (default: %s)\n"
              "                              <geo> Format: ((width[%%c]?[xX]height[%%l]?))?([+-]xOffset[+-]yOffset)?(*valign/halign)?\n"
              "                              Where:\n"
              "                                 valign: one of top,middle,bottom or none\n"
              "                                 halign: one of left,center,right or none\n"
              "  -t, --text-align=<align>    Text alignment within the OSD window(default: %s)\n"
              "                              <align> Format: <halign/valign>\n"
              "                                 valign: one of top,middle,bottom or none\n"
              "                                 halign: one of left,center,right or none\n"
              "  -f, --font=<font>           Font for display (default: %s)\n"
              "                                  <font> is Xft font name\n"
              "  -l, --list-fonts[=<search>] List known font names\n"
              "                                  <search> is part of nerd font name\n"
              "  -c, --color=<color>         Foreground color for text (default: %s)\n"
              "  -p, --padding=<padding>     Padding for the content (default: %s)\n"
              "                                  <padding> Format: top right bottom left\n"
              "  -e, --exec=<command>        Execute given command in background\n"
              "  -b, --bg-color=color        Color for background (default: %s)\n"
              "  -a, --bg-alpha=n            Background transparency (default: %d)\n"
              "                                  <n> should between 0-100\n"
#ifdef HAVE_LIBXINERAMA
              "      --no-xinerama           Turn off xinerama support\n"
#endif
#ifdef HAVE_LIBXRANDR
              "      --no-xrandr             Turn off xrandr support\n"
#endif
#ifdef HAVE_LIBXRANDR
              "  -m, --monitor=<monitor>     Monitor to display message (default: active)\n"
              "                                   <monitor>: Either monitor number (starts with 1),\n"
              "                                              active or primary\n"
#else
#if defined(HAVE_LIBXINERAMA)
              "  -m, --monitor=<monitor>     Monitor to display message (default: 1)\n"
              "                                   <monitor>: monitor number (starts with 1),\n"
#endif
#endif
#ifdef DEBUG
              "  -D, --debug=<level>        The debug levels to be enabled\n"
              "                                   <level>: CSV of none function,locking,select,trace,value,update,all\n"
#endif
              "\n\n", geometry, text_align, font, color, padding, bg_color, bg_alpha );
}

/* vim: foldmethod=marker tabstop=2 shiftwidth=2 expandtab
 */
