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

AM_CXXFLAGS=-Wall -DDATADIR=\"@datadir@\" -DPREFIX=\"@prefix@\"

bin_PROGRAMS = gnoscan 

INCLUDES = $(GTKMM_CFLAGS) $(GNOMEMM_CFLAGS)

gnoscan_LDADD = $(GNOMEMM_LIBS)

# Don't forget to update NEWS and ChangeLog
gnoscan_SOURCES = 	GnoScan.cc GnoMainWindow.cc TcpScan.cc LicenseBox.cc \
                    PreferencesBox.cc Preferences.cc \
	       			GnoScan.hh GnoMainWindow.hh TcpScan.hh LicenseBox.hh \
                    PreferencesBox.hh Preferences.hh \
                    getopt.c getopt.h getopt1.c
