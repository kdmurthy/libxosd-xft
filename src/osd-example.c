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
