// GnoScan.cc - source file for the GnoScan program
// Copyright (c) 2001  Andreas Bauer <baueran@users.berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.


#include <gtk--/main.h>
#include <gnome--/main.h>
#include <iostream>
#include <string>
extern "C" {
#include <stdlib.h>
#include <glib.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
}
#ifdef HAVE_GETOPT_H
extern "C" {
#include <getopt.h>
}
#else
extern "C" {
#include "getopt.h"
}
#endif
#include "config.h"
#include "GnoScan.hh"
#include "GnoMainWindow.hh"
#include "Preferences.hh"


using namespace std;

int main(int argc, char *argv[])
{
  int c = 0;
  extern char* optarg;
  char* dummyArg[] = { "" };
  static string gnoscanrc, prefsFile;

  static struct option long_options[] = {
    {"gnoscanrc", 1, NULL, VALUE_GNOSCANRC},
    {"help",      0, NULL, VALUE_HELP},
    {"version",   0, NULL, VALUE_VERSION},
    {0, 0, 0, 0}
  };

  // Scan command line parameters
  while ( (c = getopt_long(argc, argv, "G:hV", long_options, NULL)) != -1 ) {
    switch (c) {
    case 'h':
    case VALUE_HELP:
      cout << "Use GnoScan to scan and secure your network." << endl;
      cout << endl;
      cout << "Usage: " << PACKAGE << " [OPTION]..." << endl;
      cout << endl;
      cout << "Options:" << endl;
      cout << "  -h, --help               Display this help information" << endl;
      cout << "  -V, --version            Display version information" << endl;
      cout << "  -G, --gnoscanrc=FILE     Location of preferences and settings file" << endl;
      cout << endl;
      cout << "Report bugs to <baueran@users.berlios.de>." << endl;
      return(0);
      break;
    case 'G':
    case VALUE_GNOSCANRC:
      gnoscanrc = optarg;
      break;
    case 'V':
    case VALUE_VERSION:
      cout << PACKAGE << " " << VERSION << endl;
      cout << endl;
      cout << "Copyright (c) 2001  Andreas Bauer  <baueran@users.berlios.de>" << endl;
      cout << endl;
      cout << "This is free software; see the source for copying conditions.  There is NO" << endl;
      cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
      return(0);
      break;
    default:
      // Unrecognized option
      cerr << "Try '" << argv[0] << " --help' for more information." << endl;
      return(-1);
      break;
    }
  }

  // Locate .gnoscanrc depending on whether the user gave one on the command line or not
  if (!gnoscanrc.length()) {
    if (getenv("HOME"))
      prefsFile = (string)getenv("HOME") + (string)"/.gnoscanrc";
    else
      prefsFile = (string)"/home/" + (string)getpwuid(getuid())->pw_name + (string)"/.gnoscanrc";
  }
  else
     prefsFile = gnoscanrc.c_str();

  try {
    // Init threads
    g_thread_init(NULL);

    // Set defaults (or read) preferences
    pref::Preferences prefs(prefsFile);

    // Initialise the GnoScan kit
    Gnome::Main kit((string)PACKAGE, (string)VERSION, 1, dummyArg);

    // Initialise main window and start message loop
    gnomain::GnoMainWindow mainWindow((string)PACKAGE, &prefs);
    gdk_threads_enter();
    kit.run();
    gdk_threads_leave();

    // End program
    return(0);
  }
  catch (pref::MalformedPrefsFile) {
    cerr << (string)PACKAGE << ": Error: MalformedPrefsFile was thrown. Check the syntax of your rcfile containing the programs preferences." << endl;
    return(-1);
  }
  catch (gnomain::PThreadException) {
    cerr << (string)PACKAGE << ": Error: Exception occured while trying to (un)lock mutex. Consider reporting a bug." << endl;
    return(-1);
  }
  catch (...) {
    cerr << (string)PACKAGE << ": Error: Unknown exception was thrown. Consider reporting a bug." << endl;
    return(-1);
  }
}
