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
#ifdef HAVE_GETOPT_H
extern "C" {
#include <getopt.h>
}
#else
extern "C" {
#include <stdlib.h>
#include "getopt.h"
}
#endif
#include "config.h"
#include "GnoScan.hh"
#include "GnoMainWindow.hh"


using namespace std;

int main(int argc, char *argv[])
{
  int c = 0, option_index = 0;
  extern char *optarg;
  string gnoscanrc;

  static struct option long_options[] = {
    {"help", 0, NULL, VALUE_HELP},
    {"gnoscanrc", 1, NULL, VALUE_GNOSCANRC},
    {"version", 0, NULL, VALUE_VERSION},
    {0, 0, 0, 0}
  };

  // Scan command line parameters
  while ( (c = getopt_long(argc, argv, "hV", long_options, &option_index)) != -1 ) {
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
      cout << "      --gnoscanrc=FILE     Location of preferences and settings file" << endl;
      cout << endl;
      cout << "Report bugs to <baueran@users.berlios.de>." << endl;
      return(0);
      break;
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
    }
  }

  // Initialise the GnoScan kit
  Gnome::Main kit((string)PACKAGE, (string)VERSION, argc, argv);
  
  try {
    // Initialise main window and start message loop
    gnomain::GnoMainWindow mainWindow((string)PACKAGE);
    kit.run();

    // End program
    return(0);
  }
  catch (...) {
    cerr << "Some exception was thrown. Bad luck!" << endl;
    return(-1);
  }
}
