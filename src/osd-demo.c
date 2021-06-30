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

typedef enum
{
  DEMO_none = 0,
  DEMO_color = (1<<0),
  DEMO_alpha = (1<<1),
  DEMO_size = (1<<2),
  DEMO_position = (1<<3),
  DEMO_padding = (1<<4),
  DEMO_align = (1<<5),
  DEMO_text_align = (1<<6)
} osd_demo;

int all_demo = False;

static struct option long_options[] = {
#ifdef DEBUG
    {"debug",           1, NULL, 'D'},
#endif
    {"alpha",           0, NULL, 'a'},
    {"align",           0, NULL, 'A'},
    {"color",           0, NULL, 'c'},
    {"font",            1, NULL, 'f'},
    {"position",        0, NULL, 'p'},
    {"padding",         0, NULL, 'P'},
    {"size",            0, NULL, 's'},
    {"text-align",      0, NULL, 't'},
    {"all",             0, &all_demo, True },
    {"help",            0, NULL, 'h'},

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
char*     font        = "mixed:size=36";
char*     color       = "red";
char*     bg_color    = "black";
int       bg_alpha    = 100;
char*     padding     = "0";
int       delay_millis = 1000;

#if defined(HAVE_LIBXINERAMA) || defined(HAVE_LIBXRANDR)
int       monitor = -1;
#endif
char*     geometry = "800x80+0+0*middle/center";
char*     text_align = "center/middle";
#ifdef DEBUG
char*     debug_level = NULL;
#endif
char*     message = "~~~ Linux Rocks ~~~";


osd_demo  demo = DEMO_none;

static void demo_color(xosd_xft* osd, char **argv);
static void demo_alpha(xosd_xft* osd, char **argv);
static void demo_size(xosd_xft* osd, char **argv);
static void demo_align(xosd_xft* osd, char **argv);
static void demo_text_align(xosd_xft* osd, char **argv);
static void demo_position(xosd_xft* osd, char **argv);
static void demo_padding(xosd_xft* osd, char **argv);
static void help(char **argv, char *error);

void
pause_demo()
{
  char b[200];
  printf("Press ENTER to continue...");
  fflush(stdout);
  fgets(b, 200, stdin);
}

void
reset_osd(xosd_xft* osd, char** argv)
{
  osd_geometry g, *parsed = &g;
  if(osd_parse_geometry(geometry, text_align, parsed) == NULL) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  osd_set_geometry(osd, parsed);
  osd_set_font(osd, font);
  osd_set_monitor(osd, monitor);
  osd_set_padding(osd, padding);
  osd_set_color(osd, color);
  osd_set_bgcolor(osd, bg_color, bg_alpha);
}

int main(int argc, char *argv[])
{
  if (setlocale(LC_ALL, "") == NULL || !XSupportsLocale())
    fprintf(stderr, "Locale not available, expect problems with fonts.\n");

  while (1)
  {
    int option_index = 0;
    int c =
        getopt_long(argc, argv, "D:f:caApPsth?",
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
    case 'a':
      demo |= DEMO_alpha;
      break;
    case 'A':
      demo |= DEMO_align;
      break;
    case 'c':
      demo |= DEMO_color;
      break;
#ifdef DEBUG
    case 'D':
      debug_level = optarg;
      break;
#endif
    case 'p':
      demo |= DEMO_position;
      break;
    case 'P':
      demo |= DEMO_padding;
      break;
    case 's':
      demo |= DEMO_size;
      break;
    case 't':
      demo |= DEMO_text_align;
      break;
    case '?':
    case 'h':
    default:
      help(argv, NULL);
      return EXIT_SUCCESS;
    }
  }

  if(all_demo) demo = -1;
  if(demo == DEMO_none) {
    help(argv, "No demo option provided");
    return EXIT_FAILURE;
  }

  xosd_xft *osd = osd_create();
  reset_osd(osd, argv);
  osd_set_xinerama(osd, use_xinerama);
  osd_set_xrandr(osd, use_xrandr);
#ifdef DEBUG
  osd_set_debug_level(debug_level);
#endif

  if(demo & DEMO_color) {
    reset_osd(osd, argv);
    demo_color(osd, argv);
  }
  if(demo & DEMO_alpha) {
    reset_osd(osd, argv);
    demo_alpha(osd, argv);
  }
  if(demo & DEMO_size) {
    reset_osd(osd, argv);
    demo_size(osd, argv);
  }
  if(demo & DEMO_align) {
    reset_osd(osd, argv);
    demo_align(osd, argv);
  }
  if(demo & DEMO_text_align) {
    reset_osd(osd, argv);
    demo_text_align(osd, argv);
  }
  if(demo & DEMO_position) {
    reset_osd(osd, argv);
    demo_position(osd, argv);
  }
  if(demo & DEMO_padding) {
    reset_osd(osd, argv);
    demo_padding(osd, argv);
  }
  osd_destroy(osd);
  return EXIT_SUCCESS;
}

static void
demo_color(xosd_xft* osd, char **argv)
{
  char *colors[] =    { "#f96167", "#f9de42" };
  char *bg_colors[] = { "#fce77d", "#292826" };

  if(osd_display(osd, message, strlen(message)) == -1) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < sizeof(colors)/sizeof(char*); i++) {
    printf("Colors { bg: %s, fg: %s }\n", bg_colors[i], colors[i]);
    osd_set_color(osd, colors[i]);
    osd_set_bgcolor(osd, bg_colors[i], bg_alpha);
    pause_demo();
  }
}

static void
demo_alpha(xosd_xft* osd, char **argv)
{
  if(osd_display(osd, message, strlen(message)) == -1) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  for(int alpha = bg_alpha; alpha >= 0; alpha-=10) {
    printf("Setting background alpha to: %d\n", alpha);
    osd_set_bgcolor(osd, bg_color, alpha);
    pause_demo();
  }
}

static void
tryout_widths(xosd_xft* osd, osd_geometry *g, int per, int chars, int* widths, int len)
{
    g->width_per = per;
    g->width_chars = chars;
    for(int i = 0; i < len; i++) {
      printf("Setting width to %d\n", widths[i]);
      g->width = widths[i];
      osd_set_geometry(osd, g);
      pause_demo();
    }
}

static void
tryout_heights(xosd_xft* osd, osd_geometry *g, int per, int lines, int* heights, int len)
{
    g->height_per = per;
    g->height_lines = lines;
    for(int i = 0; i < len; i++) {
      printf("Setting height to %d\n", heights[i]);
      g->height = heights[i];
      osd_set_geometry(osd, g);
      pause_demo();
    }
}

static void
demo_size(xosd_xft* osd, char **argv)
{
  char *geometry = "100%x80+0+0";
  osd_geometry to_parse, *g = &to_parse;
  g = osd_parse_geometry(geometry, text_align, g);
  if(g == NULL) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  osd_set_geometry(osd, g);
  if(osd_display(osd, message, strlen(message)) == -1) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  printf("***** WIDTH as percentage of screen size *****\n");
  {
    int widths[] = { 40, 60, 80, 100 };
    tryout_widths(osd, g, 1, 0, widths, sizeof(widths)/sizeof(int));
  }
  printf("***** WIDTH as number of characters *****\n");
  {
    int widths[] = { 40, 60, 80, 100 };
    tryout_widths(osd, g, 0, 1, widths, sizeof(widths)/sizeof(int));
  }
  printf("***** WIDTH as number of pixels *****\n");
  {
    int widths[] = { 400, 600, 800, 1000 };
    tryout_widths(osd, g, 0, 0, widths, sizeof(widths)/sizeof(int));
  }
  printf("***** HEIGHT as percentage of screen size *****\n");
  {
    int heights[] = { 40, 60, 80, 100 };
    tryout_heights(osd, g, 1, 0, heights, sizeof(heights)/sizeof(int));
  }
  printf("***** HEIGHT as number of lines *****\n");
  {
    int heights[] = { 4, 6, 8, 10 };
    tryout_heights(osd, g, 0, 1, heights, sizeof(heights)/sizeof(int));
  }
  printf("***** HEIGHT as number of pixels *****\n");
  {
    int heights[] = { 400, 600, 800, 1000 };
    tryout_heights(osd, g, 0, 0, heights, sizeof(heights)/sizeof(int));
  }
}

static char *osd_valign_names[] = {"top", "bottom", "middle", "none"};
static char *osd_halign_names[] = {"left", "center", "right", "none"};

static void
demo_align(xosd_xft* osd, char **argv)
{
  char* geometry = "100%x80+0+0";
  osd_geometry to_parse, *g = &to_parse;
  g = osd_parse_geometry(geometry, text_align, g);
  if(g == NULL) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  osd_set_geometry(osd, g);
  if(osd_display(osd, message, strlen(message)) == -1) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  printf("***** Vertical Align *****\n");
  for(int i = 0; i < 3; i++) {
      printf("Setting valign to %s\n", osd_valign_names[i]);
      g->valign = i ;
      osd_set_geometry(osd, g);
      pause_demo();
  }
  printf("***** Horizontal Align *****\n");
  geometry = "60%x80+0+0";
  g = &to_parse;
  g = osd_parse_geometry(geometry, text_align, g);
  if(g == NULL) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  osd_set_geometry(osd, g);
  for(int i = 0; i < 3; i++) {
      printf("Setting halign to %s\n", osd_halign_names[i]);
      g->halign = i ;
      osd_set_geometry(osd, g);
      pause_demo();
  }
}

static void
demo_text_align(xosd_xft* osd, char **argv)
{
  char *geometry = "100%x180+0+0";
  osd_geometry to_parse, *g = &to_parse;
  g = osd_parse_geometry(geometry, text_align, g);
  if(g == NULL) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  osd_set_geometry(osd, g);
  if(osd_display(osd, message, strlen(message)) == -1) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  printf("***** Text Vertical Align *****\n");
  for(int i = 0; i < 3; i++) {
      printf("Setting text valign to %s\n", osd_valign_names[i]);
      g->text_valign = i ;
      osd_set_geometry(osd, g);
      pause_demo();
  }
  printf("***** Text Horizontal Align *****\n");
  for(int i = 0; i < 3; i++) {
      printf("Setting text halign to %s\n", osd_halign_names[i]);
      g->text_halign = i ;
      osd_set_geometry(osd, g);
      pause_demo();
  }
}

static void
demo_position(xosd_xft* osd, char **argv)
{
  char* geometry = "400x80+0+0";
  osd_geometry to_parse, *g = &to_parse;
  g = osd_parse_geometry(geometry, text_align, g);
  if(g == NULL) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  osd_set_geometry(osd, g);
  if(osd_display(osd, message, strlen(message)) == -1) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  printf("***** Position (Offsets) *****\n");
  for(int xneg = 0; xneg < 2; xneg++) {
    for(int yneg = 0; yneg < 2; yneg++) {
      for(int offset = 0; offset < 31; offset += 30) {
        g->xneg = xneg;
        g->yneg = yneg ;
        g->xoffset = g->yoffset = offset ;
        printf("Setting geometry to %dx%d%s%d%s%d\n",
          g->width, g->height, g->xneg ? "-" : "+", g->xoffset, g->yneg ? "-" : "+", g->yoffset);
        osd_set_geometry(osd, g);
        pause_demo();
      }
    }
  }
}
static void
demo_padding(xosd_xft* osd, char **argv)
{
  char* paddings[] = { "0", "30", "30 20", "30 20 15", "30 20 15 10" };
  char* geometry = "600x180+0+0*middle/center";
  char* text_align = "left/top";
  osd_geometry to_parse, *g = &to_parse ;
  g = osd_parse_geometry(geometry, text_align, g);
  if(g == NULL) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  osd_set_geometry(osd, g);
  if(osd_display(osd, message, strlen(message)) == -1) {
    help(argv, osd_error);
    exit(EXIT_FAILURE);
  }
  printf("***** Padding *****\n");
  for(int i = 0; i < 5; i++) {
    printf("Setting padding to %s\n", paddings[i]);
    osd_set_padding(osd, paddings[i]);
    pause_demo();
  }
}

static void
help(char **argv, char* error)
{
  FILE *out = error ? stderr : stdout ;
  if(error) {
    fprintf(stderr, "Error: %s\n", error);
  }
  fprintf(out, "Usage: %s [OPTION] message...\n", argv[0]);
  fprintf(out, "Version: %s\n", XOSD_XFT_VERSION);
  fprintf(out,
          "Display a given message on top of the display\n"
          "\n"
          "  -h, --help                 Show this help\n"
          "  -f, --font=<font>          Font for display (default: %s)\n"
          "                                  <font> is Xft font name\n"
          "  -c, --color                Demoing color options\n"
          "  -a, --alpha                Demoing background alpha\n"
          "  -A, --align                Demoing various alignment options for OSD window\n"
          "  -p, --position             Demoing various position options\n"
          "  -P, --padding              Demoing various padding options\n"
          "  -s, --size                 Demoing various options for specifying size\n"
          "  -t, --text-align           Demoing various text alignment options\n"
#ifdef DEBUG
          "  -D, --debug=<level>        The debug levels to be enabled\n"
          "                                   <level>: CSV of none function,locking,select,trace,value,update,all\n"
#endif
          "\n\n", font );
}

/* vim: foldmethod=marker tabstop=2 shiftwidth=2 expandtab
 */
