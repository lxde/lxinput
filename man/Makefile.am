man_MANS = \
	lxinput.1

man_XMANS = \
	lxinput.xml

EXTRA_DIST = \
	$(man_MANS) \
	$(man_XMANS)


if ENABLE_REGENERATE_MAN

lxinput.1: lxinput.xml
	$(XSLTPROC) -nonet http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $<        

CLEANFILES = $(man_MANS) 

endif


