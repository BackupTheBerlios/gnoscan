# gnoscan 
#
# Copyright (c) 2001  Andreas Bauer <baueran@users.berlios.de> 
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
# USA.


AUTOMAKE_OPTIONS = dist-shar dist-zip dist-tarZ

SUBDIRS = macros src

EXTRA_DIST = TODO

# build a distribution snapshot
# TODO:
# add a snap documentation target
# check to see if there is already a previous snapshot there, and don't overwrite it!
@MAINT@snapshot: README-snapshot maintainer-clean
@MAINT@	@a=`date -R` ;\
@MAINT@	echo "Last built on $$a" >> README-snapshot
@MAINT@	@d=`pwd` ;\
@MAINT@	d=`basename $$d` ;\
@MAINT@	echo $$d ;\
@MAINT@	cd .. ;\
@MAINT@	tar -cvzf $$d-`date +%m%d%y`.tar.gz $$d ;\
@MAINT@	mv $$d-`date +%m%d%y`.tar.gz $$d/ ;\
@MAINT@	cd $$d

@MAINT@alldist:
@MAINT@	${MAKE} distcheck
@MAINT@	${MAKE} dist-shar
@MAINT@	${MAKE} dist-zip
@MAINT@	${MAKE} dist-tarZ

@MAINT@cvsclean: maintainer-clean
@MAINT@	@-rm -f `find . -name Makefile.in`
@MAINT@	@-rm -f configure aclocal.m4 config.h.in stamp-h.in
@MAINT@	@-rm -f mkinstalldirs missing install-sh COPYING
@MAINT@	@-rm -rf @PACKAGE@-@VERSION@*
@MAINT@	@echo "================================================="
@MAINT@	@echo "Don't forget your ChangeLog and NEWS entries ...."
@MAINT@	@echo "================================================="
