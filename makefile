# -*- Mode: makefile -*-
# Makefile for GAlib
# Copyright (c) 1996-2005 Matthew Wall, all rights reserved
#
# If you need to customize the build of galib, you should first modify the
# variables in the makevars file.

GALIB_VERSION=2.4.7
GALIB_VER=247
TMPDIR=/var/tmp
RELDIR=$(TMPDIR)/galib$(GALIB_VER)

all: lib ex

lib:
	cd ga; $(MAKE)

ex:
	cd examples; $(MAKE)

test: lib ex
	cd examples; $(MAKE) test

install:
	cd ga; $(MAKE) install

uninstall:
	cd ga; $(MAKE) uninstall

clean:
	cd ga; $(MAKE) clean
	cd examples; $(MAKE) clean

release: clean 
	rm -rf $(RELDIR)
	mkdir -p $(RELDIR)
	cp -rp * $(RELDIR)
	rm -rf `find $(RELDIR) -name CVS`
	rm -rf `find $(RELDIR) -name .svn`
	rm -f `find $(RELDIR) -name "*~"`
	echo $(GALIB_VERSION) > $(RELDIR)/VERSION
	perl -pi -e 's/evision: \d+\.\d+ /evision: $(GALIB_VERSION) /' $(RELDIR)/ga/gaversion.h
	perl -pi -e 'chop($$dt=`date +"%Y/%m/%d %H:%M:%S"`); s/Date: ..\/..\/.. ..:..:.. /Date: $$dt /' $(RELDIR)/ga/gaversion.h
	cd $(RELDIR)/..; tar cvfz galib$(GALIB_VER).tgz galib$(GALIB_VER) > $(TMPDIR)/galib$(GALIB_VER)-manifest-tar.txt
	cd $(RELDIR)/..; zip -r galib$(GALIB_VER).zip galib$(GALIB_VER) > $(TMPDIR)/galib$(GALIB_VER)-manifest-zip.txt
	@echo "  GAlib $(GALIB_VERSION) has been released to $(TMPDIR)"
