SUFFIXES = .md
.md:
	@if command -v pandoc &> /dev/null ; \
	then \
		pandoc -s -t man -o $@ $< ; \
	else \
		@echo Pandoc not installed. Just touching and going ahead ; \
		touch $@ ; \
	fi

man_MANS = osd-echo.1 osd-cat.1 osd-demo.1 \
							xosd-xft.3 osd_create.3 osd_destroy.3 osd_display.3 osd_hide.3 \
							osd_parse_geometry.3 osd_set_bgcolor.3 osd_set_color.3 osd_set_debug_level.3 \
							osd_set_font.3 osd_set_geometry.3 osd_set_monitor.3 osd_set_number_of_lines.3 \
							osd_set_padding.3 osd_set_shadowcolor.3 osd_set_shadowoffset.3 osd_set_xinerama.3 \
							osd_set_xrandr.3 osd_show.3

EXTRA_DIST = ${man_MANS}

# vim: foldmethod=marker tabstop=2 shiftwidth=2 noexpandtab
