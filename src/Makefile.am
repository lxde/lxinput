INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"/lxinput\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(GTK_CFLAGS) \
	$(XLIB_CFLAGS)
	-Werror-implicit-function-declaration

bin_PROGRAMS = lxinput

lxinput_SOURCES = \
	lxinput.c

lxinput_LDADD = \
	$(GTK_LIBS) \
	$(XLIB_LIBS) \
	$(INTLLIBS)

noinst_PROGRAMS=xml-purge
xml_purge_SOURCES=xml-purge.c
xml_purge_CFLAGS=$(GTK_CFLAGS)
xml_purge_LDADD=$(GTK_LIBS)
