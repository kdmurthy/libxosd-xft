% XOSD-XFT(1) | XOSD-XFT General Commands
% Dakshinamurthy Karra
% Jun 28 2021

# NAME

osd-show-key, osd-show-file, osd-demo - Show information in a OSD window

# SYNOPSIS

osd-show-key [*options*] *message*

osd-show-file [*options*] [*file*]

osd--demo [*options*]

# DESCRIPTION

`osd-show-key` shows a message in a translucent window on the screen.
`osd-show-key` can understand unicode escape sequences in the message and
can also understand glyph names from Nerd fonts.

`osd-show-file` shows a given file in a OSD window. If a file is not given
`osd-show-file` reads from the standard input.

`osd-demo` is a small program that shows the capabilities of `xosd-xft`
library.

Both `osd-show-key` and `osd-show-file` provides various options to control
the colors, font and geometry used to display the information.

# OPTIONS

-g *GEOMETRY*, \--geometry=*GEOMETRY*
:   Specify the geometry for the window. The geometry contains the size,
    offset and alignment specifications.

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
-t *ALIGN*, \--text-align=*ALIGN*
:   Text alignment within the OSD window

```
    alignment: <halign>/<valign>
      halign: one of left,center,right or none
      valign: one of top,middle,bottom or none
```

-f *FONT*, \--font=*FONT
:   Font for display. *FONT* is Xft font name.

-c *COLOR*, \--color=*COLOR*
:   Foreground color for text. *COLOR* is a X11 color name.

-p *PADDING*, \--padding=*PADDING*
:   Padding for the content

```
    padding: top right bottom left
      All as integers. When later values are absent, the previous values are used
      as in CSS specification.
```

-b *COLOR*, \--bg-color=*COLOR*
:   Color for background (default: black)

-a *ALPHA*, \--bg-alpha=*ALPHA*
:   Background transparency
```
    ALPHA value should be between 0-100
```

\--no-xinerama
:   Turn off xinerama support

\--no-xrandr
:   Turn off xrandr support

-m *MONITOR*, \--monitor=*MONITOR*
:   Monitor to display message

```
    MONITOR is either a number of the monitor (the numbering starts with 1). You can
    specify either `active` or `primary` to choose either of the active or primary
    monitors.
```

Besides the above options `osd-show-file` accepts the following options:

-n *NUMBER*, \--number-of-lines=*NUMBER*
:   Number of lines to display

The `osd-show-key` command accepts the following additional options:

-e *COMMAND*, \--exec=*COMMAND*
:   Executes the command using system(3) before displaying the window

-l[*SEARCH*], \--list[=*SEARCH*]
:   List the glyph names known to the command. If a SEARCH is give, lists
    glyph names that contains the search pattern.

# EXAMPLES

To display `/etc/passwd`:
```
    osd-show-file /etc/passwd
```

To override the font used to display a file:
```
    osd-show-file -f "SourceCodePro:size=14" /etc/passwd
```

To override the geometry - using 120 characters width and 30 lines height:
```
    osd-show-file -g 120cx30l /etc/passwd
```

To display volume off glyph and execute `amixer` command:
```
    osd-show-key -e 'amixer set Master toggle' :fa-volume_off:")
```

To list available glyph names that contain `volume` in them:
```
    osd-show-key -lvolume
```
