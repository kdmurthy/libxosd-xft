# XOSD-XFT - OSD for X11

Inspired by [libxosd](https://sourceforge.net/projects/libxosd/).

`xosd-xft` supports:

* Use Xft/TTF fonts
* Xrandr and Xinerama extensions
* Allows you to choose a monitor in multihead setups - including active monitor
* Use `osd-echo` to display a [Nerd Font](https://nerdfonts.com) glyph
* Use `osd-cat` to display a file

# Installation

```bash
./configure
make
sudo make install
```

# Quick Look

## Using osd-echo

To display volume off glyph and execute `amixer` command:

```bash
    osd-echo -e 'amixer set Master off' :fa-volume_off:")
```

To list available glyph names that contain `volume` in them:

```
    osd-echo -lvolume
```

## Using osd-showfile

To override the font used to display a file:
```bash
    osd-cat -f "SourceCodePro:size=14" /etc/passwd
```

The following command shows the output of uptime on the screen and updates every 5 seconds:

```bash
while true; do uptime; sleep 5; done | osd-cat --number-of-lines 1 -g 0x1l+0-0
```

## Using the library

```c
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
