// GnoMainWindow.hh - source file for the GnoScan program
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


#ifndef GNOMAINWINDOW_HH
#define GNOMAINWINDOW_HH

#define appWidth           400
#define appHeight          465

#define STD_PADDING         10
#define STD_PADDING_HALF     5

#include <vector>
#include <gnome--.h>
#include <gnome--/app.h>
#include <gnome--/about.h>
#include <gtk--/spinbutton.h>
#include <gtk--/statusbar.h>
#include <gtk--/entry.h>
#include <gtk--/main.h>
#include <gtk--/clist.h>
#include <gtk--/combo.h>
#include "LicenseBox.hh"
#include "TcpScan.hh"
#include "Preferences.hh"


namespace gnomain {

  struct scanOptions {
    Gtk::SpinButton* start;
    Gtk::SpinButton* end;
    Gtk::Entry* server;
    Gtk::Combo* netmask;
  };

  class GnoMainWindow : public Gnome::App {
  private:
    vector<Gnome::UI::Info> menuBar;
    vector<Gnome::UI::Info> menuFile;
    vector<Gnome::UI::Info> menuEdit;
    vector<Gnome::UI::Info> menuHelp;

    void init(void);
    void installMenus(void);
    void displayAboutBox(void);
    void displayLicenseBox(void);
    void displayOptions(void);
    void displayHelp(void);
    void destroyDialog(void);
    void closeWindow(void);
    void closeWindowAndSave(void);
    void startScan(scanOptions);

  public:
    GnoMainWindow(string, pref::Preferences*);
    ~GnoMainWindow();
    
    gint delete_event_impl(GdkEventAny*) {
      closeWindowAndSave();
      return 0;
    }
  };
  
  // Processes
  void* helpProcess(void*);
  void* scanProcess(void*);
  void* scanResultProcess(void*);
  class PThreadException { };

}

#endif
