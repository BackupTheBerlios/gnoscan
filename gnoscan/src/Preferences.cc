// Preferences.cc - source file for the GnoScan program
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


#include <list>
#include <string>
#include <strstream>
#include <fstream>
#include <vector>
#include "config.h"
#include "GnoScan.hh"
#include "Preferences.hh"

using namespace std;

namespace pref {

  Preferences::Preferences(string file) {
    fileName = file;
    ifstream prefsFile(fileName.c_str());
    string curLine;

    // Defaults
    sourcePort = 0;
    extraInfo = TRUE;
    specificPort = FALSE;
    timeOuts = 3;

    // Load settings
    if (prefsFile) {
      try {
	while (getline(prefsFile, curLine)) {
	  if (curLine[0] == '#')
	    continue;
	  
	  if (curLine.find("SOURCE_PORT=", 0) == 0) {
	    sourcePort = atoi( (*(getPrefValues(curLine)).begin()).c_str() );
	  }
	  else if (curLine.find("TIMEOUTS=", 0) == 0) {
	    timeOuts = atoi( (*(getPrefValues(curLine)).begin()).c_str() );
	  }
	  else if (curLine.find("SPECIFIC_PORT=", 0) == 0) {
	    if (*(getPrefValues(curLine)).begin() == "yes")
	      specificPort = TRUE;
	    else if (*(getPrefValues(curLine)).begin() == "no")
	      specificPort = FALSE;
	    else {
	      cerr << (string)PACKAGE << ": Error: Specific port could not be set in the rcfile." << endl;
	      throw MalformedPrefsFile();      
	    }
	  }
	  else if (curLine.find("EXTRA_INFO=", 0) == 0) {
	    if (*(getPrefValues(curLine)).begin() == "yes")
	      extraInfo = TRUE;
	    else if (*(getPrefValues(curLine)).begin() == "no")
	      extraInfo = FALSE;
	    else {
	      cerr << (string)PACKAGE << ": Error: Extra info could not be set in the rcfile." << endl;
	      throw MalformedPrefsFile();      
	    }
	  }
	  else {
	    cerr << (string)PACKAGE << ": Error: Malformed rcfile for preferences. Program start aborted." << endl;
	    throw MalformedPrefsFile();
	  }

	  // Store prefs in memory, as long as there are not too many keywords this works ok
	  prefsFileContent.push_back(curLine);
	}
      }
      catch (...) {
	prefsFile.close();
	throw;
      }

      prefsFile.close();
    }
    else {
      if (!create(fileName))
	cerr << (string)PACKAGE << ": Error: Could not locate or create rcfile for preferences. Check file permissions and paths." << endl;
    }
  }

  
  Preferences::~Preferences() {
  }


  // Gets the values from an entry inside the preferences file
  // e.g. TEST=value1, value2
  // would result in returning 'value1' and 'value2' (I've 'stolen' this from my own applications Archiebald and Mailfilter)
  list<string> Preferences::getPrefValues(string prefLine) {
    list<string> parsedValues;
    int startSearch = prefLine.find('=') + 1;
    int newPos = 0;
    
    if (startSearch != -1) {
      for (int i = startSearch; i <= (int)prefLine.length(); i++) {
	newPos = prefLine.find(',', i);
	
	if (newPos == -1) {
	  parsedValues.push_back( prefLine.substr( i, prefLine.length() ) );
	  break;
	}
	else {
	  parsedValues.push_back( prefLine.substr( i, newPos - i ) );
	  i = newPos + 1;
	}
      }
      
      return parsedValues;
    }
    else
      throw MalformedPrefsFile();
  }

  
  // Save the preferences file
  bool Preferences::save(string prefsFileName) {
    ofstream prefsFile(prefsFileName.c_str());
    vector<string>::iterator curLine = prefsFileContent.begin();

    if (prefsFile) {
      while ( curLine != prefsFileContent.end() ) {
	if ((*curLine).find("SOURCE_PORT=", 0) == 0) {
	  strstream sourceStream;
	  sourceStream << sourcePort << ends;
	  setPrefValue(curLine, sourceStream.str());
	}
	else if ((*curLine).find("TIMEOUTS=", 0) == 0) {
	  strstream sourceStream;
	  sourceStream << timeOuts << ends;
	  setPrefValue(curLine, sourceStream.str());
	}
	else if ((*curLine).find("EXTRA_INFO=", 0) == 0) {
	  if (extraInfo)
	    setPrefValue(curLine, "yes");
	  else
	    setPrefValue(curLine, "no");
	}
	else if ((*curLine).find("SPECIFIC_PORT=", 0) == 0) {
	  if (specificPort)
	    setPrefValue(curLine, "yes");
	  else
	    setPrefValue(curLine, "no");
	}
	
	prefsFile << *curLine << endl;      // Append line to preferences file
	curLine++;                          // Onwards with the next line...
      }

      prefsFile.close();
      return TRUE;
    }
    else
      return FALSE;
  }


  bool Preferences::create(string file) {
    ofstream newPrefsFile(file.c_str());
    
    if (newPrefsFile) {
      newPrefsFile << "SOURCE_PORT=0" << endl;
      prefsFileContent.push_back("SOURCE_PORT=0");

      newPrefsFile << "TIMEOUTS=3" << endl;
      prefsFileContent.push_back("TIMEOUTS=3");

      newPrefsFile << "EXTRA_INFO=yes" << endl;
      prefsFileContent.push_back("EXTRA_INFO=yes");

      newPrefsFile << "SPECIFIC_PORT=no" << endl;
      prefsFileContent.push_back("SPECIFIC_PORT=no");

      newPrefsFile.close();
      return TRUE;
    }
    else
      return FALSE;
  }


  // Sets the new preferences value for a certain parameter
  void Preferences::setPrefValue(vector<string>::iterator& curLine, string newValue) {
    (*curLine).replace((*curLine).find('=') + 1, (*curLine).length(), newValue);
  }

  
  // Returns 'true' if one specific source port was chosen
  bool Preferences::useSpecificSourcePort(void) {
    if (specificPort)
      return TRUE;
    else
      return FALSE;
  }

  
  // Returns source port
  int Preferences::sourcePortValue(void) {
    return sourcePort;
  }


  bool Preferences::extraInfoValue(void) {
    return extraInfo;
  }


  void Preferences::setExtraInfoValue(bool newVal) {
    extraInfo = newVal;
  }


  void Preferences::setUseSpecificSourcePort(bool newVal) {
    specificPort = newVal;
  }

  
  void Preferences::setSourcePortValue(int newVal) {
    sourcePort = newVal;
  }


  string Preferences::getFileName(void) {
    return fileName;
  }


  int Preferences::maxTimeOuts(void) {
    return timeOuts;
  }


  void Preferences::setMaxTimeOuts(int newVal) {
    timeOuts = newVal;
  }

}
