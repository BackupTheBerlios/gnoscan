// Preferences.hh - source file for the GnoScan program
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

#ifndef PREFERENCES_HH
#define PREFERENCES_HH

#include <list>
#include <string>
#include <vector>

namespace pref {

  class Preferences {
  private:
    // Variables
    bool extraInfo;
    int sourcePort;
    vector<string> prefsFileContent;

    // Functions
    list<string> getPrefValues(string);
    bool save(string);
    void setPrefValue(vector<string>::iterator&, string);

  public:
    Preferences(string);
    ~Preferences();
    bool oneSourcePort(void);
    bool noSourcePort(void);
    int sourcePortValue(void);
    bool extraInfoValue(void);
  };

  // Exceptions
  class MalformedPrefsFile { };
}

#endif
