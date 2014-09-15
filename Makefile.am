ACLOCAL_AMFLAGS = -I .

NULL=

SUBDIRS = src po man

uidir = $(datadir)/lxinput
ui_DATA = data/lxinput.ui

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
	$(NULL)
