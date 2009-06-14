
dist_man_MANS = lxinput.1

EXTRA_DIST = lxinput.sgml

dist-hook: lxinput.1

lxinput.1: lxinput.sgml
	(test -f "`which docbook-to-man`" && docbook-to-man $< > $@) || \
	(test -f "`which db2man`" && db2man $< > $@) || \
	(test -f "`which docbook2man`" && docbook2man $< && mv LXINPUT.1 $@)
