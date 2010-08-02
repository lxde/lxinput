NULL=

SUBDIRS = src po man

ui_in_files= \
	data/lxinput.glade \
	$(NULL)
uidir=$(datadir)/lxinput
ui_DATA = $(ui_in_files:.glade=.ui)

desktopdir=$(datadir)/applications
desktop_in_files = \
	data/lxinput.desktop.in \
	$(NULL)
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@


iconsdir=$(datadir)/lxinput
icons_DATA= \
	data/input-keyboard.png \
	data/input-mouse.png

EXTRA_DIST = \
	$(ui_DATA) \
	$(desktop_DATA) \
	$(icons_DATA) \
	$(NULL)

CLEANFILES = \
	$(ui_DATA) \
	$(NULL)

%.ui: %.glade
	cp $< $@
	$(top_builddir)/src/xml-purge $@
