NULL=

SUBDIRS = src po man

ui_in_files= \
	data/lxinput.glade \
	$(NULL)
uidir=$(datadir)/lxinput
ui_DATA = $(ui_in_files:.glade=.ui)

desktopfiledir=$(datadir)/applications
desktopfile_DATA=data/lxinput.desktop

iconsdir=$(datadir)/lxinput
icons_DATA= \
	data/input-keyboard.png \
	data/input-mouse.png

EXTRA_DIST = \
	$(ui_DATA) \
	$(desktopfile_DATA) \
	$(icons_DATA) \
	$(NULL)

CLEANFILES = \
	$(ui_DATA) \
	$(NULL)

%.ui: %.glade
	cp $< $@
	$(top_builddir)/src/xml-purge $@
