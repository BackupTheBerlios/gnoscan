// GnoMainWindow.cc - source file for the GnoScan program
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


#include <string>
#include <vector>
#include <strstream>
#include <gtk--/box.h>
#include <gtk--/bin.h>
#include <gtk--/label.h>
#include <gtk--/entry.h>
#include <gtk--/button.h>
#include <gtk--/buttonbox.h>
#include <gtk--/editable.h>
#include <gtk--/frame.h>
#include <gtk--/separator.h>
#include <gtk--/spinbutton.h>
#include <gtk--/adjustment.h>
#include <gtk--/scrolledwindow.h>
#include <gtk--/statusbar.h>
#include <gtk--/pixmap.h>
#include <gtk--/combo.h>
#include <gtk--/clist.h>
#include <gtk--/base.h>
#include <gnome--.h>
#include <gnome--/main.h>
#include <gnome--/app.h>
#include <gnome--/about.h>
#include <gnome--/dialog.h>
extern "C" {
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
}
#include "config.h"
#include "GnoMainWindow.hh"
#include "PreferencesBox.hh"
#include "Preferences.hh"
#include "TcpScan.hh"


using SigC::slot;
using SigC::bind;

namespace gnomain {

  // Global variables, necessary due to C-style posix threads
  string helpPath = (string)"/usr/bin/gnome-help-browser " + (string)PREFIX + (string)"/doc/gnoscan/index.html";
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t scanLock = PTHREAD_MUTEX_INITIALIZER;
  vector<scan::scanResult>* results;
  Gtk::CList* scanCList;
  Gtk::Statusbar* statusBar;
  unsigned int statusBarID;
  scan::TcpScan scannerObj;
  pref::Preferences* prefs;
  scanOptions options;
  pthread_t scanThread, resultThread;
  
	
  // Temporary structure to pass on a lot of values to the scan process
  // It is only used by startScan() and the according thread routine
  struct optionsSummary {
    string server;
    string netmask;
    int start;
    int end;
    int sourcePort;
    bool useSourcePort;
    bool extraInfo;
    int timeOuts;
  };
  optionsSummary allOps;
  

  GnoMainWindow::GnoMainWindow(string name, pref::Preferences* newPrefs): Gnome::App(name, name)
  {
    try {
      prefs = newPrefs;

      // Initialise application and menus
      init();
      installMenus();
      
      // Display the application's main window and controls
      show_all();
    }
    catch (...) {
      throw;
    }
  }
  
  
  GnoMainWindow::~GnoMainWindow() {
    delete scanCList;
    delete statusBar;
  }
  

  void GnoMainWindow::init(void) {
    set_policy(FALSE, TRUE, FALSE);
    set_default_size(appWidth, appHeight);

    try {
      // Server entry-box and label
      Gtk::HBox* serverHBox = manage(new Gtk::HBox());
      Gtk::Label* serverLabel = manage(new Gtk::Label("Host:"));
      Gtk::Entry* serverEntry = manage(new Gtk::Entry(40));
      serverHBox->pack_start(*serverLabel, FALSE, FALSE, STD_PADDING);
      serverHBox->pack_start(*serverEntry, TRUE, TRUE, STD_PADDING);
      
      // Netmask combo box and label
      char *netmasks[5] = { "<check only specified host>", "255.255.255.0", "255.255.0.0", "255.0.0.0", "" };
      Gtk::HBox* netmaskHBox = manage(new Gtk::HBox());
      Gtk::Label* netmaskLabel = manage(new Gtk::Label("Netmask:"));
      Gtk::Combo* netmaskCombo = manage(new Gtk::Combo());
      netmaskHBox->pack_start(*netmaskLabel, FALSE, FALSE, STD_PADDING);
      netmaskHBox->pack_start(*netmaskCombo, TRUE, TRUE, STD_PADDING);
      netmaskCombo->set_popdown_strings(netmasks);
      netmaskCombo->get_entry()->set_editable(FALSE);

      // Port range spin buttons and frame
      Gtk::HBox* portHBox = manage(new Gtk::HBox());
      Gtk::VBox* portVBox = manage(new Gtk::VBox());
      Gtk::HBox* portButtonsHBox = manage(new Gtk::HBox(TRUE, 0));
      Gtk::Frame* portRangeFrame = manage(new Gtk::Frame("Port Range:"));
      Gtk::Label* portStartLabel = manage(new Gtk::Label("Start:"));
      Gtk::Adjustment* spinbutton1_adj = manage(new Gtk::Adjustment(0, 0, 65535, 1, 10, 10));
      Gtk::SpinButton* portStart = manage(new Gtk::SpinButton(*spinbutton1_adj, 1, 0));
      Gtk::Label* portEndLabel = manage(new Gtk::Label("End:"));
      Gtk::Adjustment* spinbutton2_adj = manage(new Gtk::Adjustment(1023, 0, 65535, 1, 10, 10));
      Gtk::SpinButton* portEnd = manage(new Gtk::SpinButton(*spinbutton2_adj, 1, 0));
      portStart->set_numeric(TRUE);
      portEnd->set_numeric(TRUE);
      portButtonsHBox->pack_start(*portStartLabel, TRUE, TRUE, STD_PADDING);
      portButtonsHBox->pack_start(*portStart, FALSE, TRUE, STD_PADDING);
      portButtonsHBox->pack_start(*portEndLabel, TRUE, TRUE, STD_PADDING);
      portButtonsHBox->pack_start(*portEnd, FALSE, TRUE, STD_PADDING);
      portVBox->pack_start(*portButtonsHBox, FALSE, FALSE, STD_PADDING);
      portRangeFrame->add(*portVBox);
      portHBox->pack_start(*portRangeFrame, TRUE, TRUE, STD_PADDING);
      portStart->set_wrap(true);
      
      // Scan result frame and list
      Gtk::VBox* scanVBox = manage(new Gtk::VBox(FALSE, STD_PADDING));
      Gtk::HBox* scanHBox = manage(new Gtk::HBox(FALSE, 0));
      Gtk::HBox* scanHBox2 = manage(new Gtk::HBox(TRUE, STD_PADDING));
      Gtk::ScrolledWindow* scanScroll = manage(new Gtk::ScrolledWindow());
      Gtk::Frame* scanFrame = manage(new Gtk::Frame("Results:"));
      scanCList = new Gtk::CList(4);
      scanCList->set_column_title(0, "Host");
      scanCList->set_column_justification(0, GTK_JUSTIFY_LEFT);
      scanCList->set_column_width(0, 80);
      scanCList->set_column_title(1, "Port");
      scanCList->set_column_justification(1, GTK_JUSTIFY_LEFT);
      scanCList->set_column_width(1, 40);
      scanCList->set_column_title(2, "Service");
      scanCList->set_column_justification(2, GTK_JUSTIFY_LEFT);
      scanCList->set_column_width(2, 80);
      scanCList->set_column_title(3, "Extra Info");
      scanCList->set_column_justification(3, GTK_JUSTIFY_LEFT);
      scanCList->set_column_width(3, 120);
      scanCList->set_usize(-1, 150);
      scanCList->column_titles_show();
      scanScroll->set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
      scanScroll->add(*scanCList);
      scanHBox->pack_start(*scanScroll, TRUE, TRUE, STD_PADDING);
      scanVBox->pack_start(*scanHBox, TRUE, TRUE, STD_PADDING);
      scanFrame->add(*scanVBox);
      scanHBox2->pack_start(*scanFrame, TRUE, TRUE, STD_PADDING);
      
      // Confirm buttons as well as a separator line
      Gtk::HBox* scanButtonBox = manage(new Gtk::HBox());
      Gtk::HButtonBox* confirmButtonBox = manage(new Gtk::HButtonBox());
      confirmButtonBox->set_border_width(STD_PADDING_HALF);
      confirmButtonBox->set_spacing(STD_PADDING);
      confirmButtonBox->set_layout(GTK_BUTTONBOX_END);
      Gtk::Separator* sep = manage(new Gtk::HSeparator()); // h-separator
      Gtk::Button* cancelButton = manage(new Gnome::StockButton(GNOME_STOCK_BUTTON_CANCEL));
      Gtk::Pixmap* scanPixmap = manage(new Gtk::Pixmap((string)DATADIR + "/pixmaps/gnoscan/tb_search.xpm"));
      Gtk::Label* scanLabel = manage(new Gtk::Label("Scan"));
      scanButtonBox->pack_start(*manage(static_cast<Gtk::Widget*>(scanPixmap)), TRUE, FALSE, 0);
      scanButtonBox->pack_start(*scanLabel, TRUE, FALSE, 0);
      Gtk::Button* scanButton = manage(new Gtk::Button());
      scanButton->add(*scanButtonBox);
      confirmButtonBox->add(*scanButton);
      confirmButtonBox->add(*cancelButton);
      
      // Status bar
      statusBar = new Gtk::Statusbar();
      statusBarID = statusBar->get_context_id((string)PACKAGE);
      statusBar->push( statusBarID, "Ready" );

      // Draw and arrange all widgets
      Gtk::VBox* vBox = manage(new Gtk::VBox());
      vBox->pack_start(*serverHBox, TRUE, FALSE, STD_PADDING);  // EXPAND, FILL, PADDING
      vBox->pack_start(*netmaskHBox, FALSE, FALSE, 0);
      vBox->pack_start(*portHBox, FALSE, FALSE, STD_PADDING);
      vBox->pack_start(*scanHBox2, FALSE, FALSE, STD_PADDING);
      vBox->pack_start(*sep, FALSE, FALSE, STD_PADDING_HALF);
      vBox->pack_start(*confirmButtonBox, FALSE, FALSE, STD_PADDING_HALF);
      vBox->pack_start(*statusBar, FALSE, FALSE, 0);
      set_contents(*vBox);

      // Set scan options
      options.start = portStart;
      options.end = portEnd;
      options.server = serverEntry;
      options.netmask = netmaskCombo;

      // Set button-defaults and connect signals
      scanButton->set_flags(GTK_CAN_DEFAULT);
      cancelButton->set_flags(GTK_CAN_DEFAULT);
      scanButton->clicked.connect(bind(slot(this, &GnoMainWindow::startScan), options));
      cancelButton->clicked.connect(slot(this, &GnoMainWindow::closeWindow));
      scanButton->grab_default();
    }
    catch (...) {
      throw;
    }
  }
  
  
  void GnoMainWindow::installMenus(void) {
    try {
      // Help menu
      menuHelp.push_back(Gnome::UI::Item("Help...", slot(this, &GnoMainWindow::displayHelp)));
      menuHelp.push_back(Gnome::UI::Separator());
      menuHelp.push_back(Gnome::UI::Item("License...", slot(this, &GnoMainWindow::displayLicenseBox)));
      menuHelp.push_back(Gnome::MenuItems::About(slot(this, &GnoMainWindow::displayAboutBox)));
      
      // Edit menu
      menuEdit.push_back(Gnome::MenuItems::Cut());
      menuEdit.push_back(Gnome::MenuItems::Copy());
      menuEdit.push_back(Gnome::MenuItems::Paste());
      menuEdit.push_back(Gnome::UI::Separator());
      menuEdit.push_back(Gnome::MenuItems::Properties(slot(this, &GnoMainWindow::displayOptions)));
      
      // File menu
      menuFile.push_back(Gnome::MenuItems::Exit(slot(this, &GnoMainWindow::closeWindowAndSave)));
      
      // Menu bar
      menuBar.push_back(Gnome::Menus::File(menuFile));
      menuBar.push_back(Gnome::Menus::Edit(menuEdit));
      menuBar.push_back(Gnome::Menus::Help(menuHelp));
      
      // Draw handle box with menu bar and menu items
      create_menus(menuBar);
    }
    catch (...) {
      throw;
    }
  }
  
  
  void GnoMainWindow::startScan(scanOptions ops) {
    string serverName = ops.server->get_text();

    // Change GUI
    scanCList->rows().clear();
    statusBar->pop(statusBarID);
    statusBar->push(statusBarID, "Scanning...");

    // Check if server was specified, etc.
    if (!serverName.length()) {
      Gnome::Dialogs::error("You have not specified a host to scan.");
      return;
    }
    
    if (ops.start->get_value_as_int() > ops.end->get_value_as_int()) {
      Gnome::Dialogs::error("Start port must be equal or greater than end port.");
      return;
    }

    if (prefs->sourcePortValue() < 1024 && getuid() && geteuid() && prefs->useSpecificSourcePort()) {
      Gnome::Dialogs::error("Only root can bind to port number range 0 - 1023.\nCheck your preferences or restart the program as root.");
      return;
    }

    // Set scan options
    allOps.server = ops.server->get_text();
    allOps.netmask = ops.netmask->get_entry()->get_text();
    allOps.start = ops.start->get_value_as_int();
    allOps.end = ops.end->get_value_as_int();
    allOps.sourcePort = prefs->sourcePortValue();
    allOps.useSourcePort = prefs->useSpecificSourcePort();
    allOps.extraInfo = prefs->extraInfoValue();
    allOps.timeOuts = prefs->maxTimeOuts();

    if (pthread_mutex_lock(&scanLock) != 0) {
      throw PThreadException();
    }
    else {
      // Create two threads, one scanning and one waiting for the results to put them in the main dialog
      if ( (pthread_create(&scanThread, NULL, scanProcess, &allOps) != 0) || 
	   (pthread_create(&resultThread, NULL, scanResultProcess, &results) != 0) ) {
	throw PThreadException();
      }
      
      if (pthread_mutex_unlock(&scanLock) != 0) {
	throw PThreadException();
      }
    }
  }
  
  
  void GnoMainWindow::closeWindow(void) {
    Gnome::Main::quit();
  }
  
  
  void GnoMainWindow::closeWindowAndSave(void) {
    if (!prefs->save(prefs->getFileName()))
      cerr << (string)PACKAGE << ": Error: Could not store rcfile for preferences. Changes you made have not been stored to disk." << endl;

    Gnome::Main::quit();
  }


  void GnoMainWindow::displayAboutBox(void) {
    vector<string> authors;
    string copyright = "Copyright (c) 2001 Andreas Bauer";
    string author = "Andreas Bauer <baueran@users.berlios.de>";
    string description = "Use GnoScan to scan and secure your network.\nSee http://gnoscan.berlios.de/ for further information.";
      
    statusBar->pop(statusBarID);
    statusBar->push(statusBarID, "About GnoScan");
    authors.push_back(author);
    
    Gnome::About* aboutBox = manage(new Gnome::About((string)PACKAGE, (string)VERSION, copyright, authors, description, 0));
    aboutBox->destroy.connect(slot(this,&GnoMainWindow::destroyDialog));
    aboutBox->run();
  }
  
  
  void GnoMainWindow::displayHelp(void) {
    try {
      if (pthread_mutex_lock(&lock) != 0) {
	throw PThreadException();
      }
      else {
	pthread_t helpThread;
	
	// Show help browser
	if (pthread_create(&helpThread, NULL, helpProcess, &helpPath) != 0) {
	  pthread_mutex_unlock(&lock);
	  throw PThreadException();
	}
	else {
	  if (pthread_mutex_unlock(&lock) != 0)
	    throw PThreadException();
	}
      }
    }
    catch (...) {
      throw;
    }
  }
  
  
  void GnoMainWindow::displayLicenseBox(void) {
    statusBar->pop(statusBarID);
    statusBar->push(statusBarID, "GnoScan License");

    LicenseBox* licenseBox = manage(new LicenseBox());
    licenseBox->destroy.connect(slot(this,&GnoMainWindow::destroyDialog));
    licenseBox->run();
  }
  

  void GnoMainWindow::destroyDialog(void) {
    statusBar->pop(statusBar->get_context_id((string)PACKAGE));
    statusBar->push(statusBar->get_context_id((string)PACKAGE), "Ready");
  }


  void GnoMainWindow::displayOptions() {
    statusBar->pop(statusBar->get_context_id((string)PACKAGE));
    statusBar->push(statusBar->get_context_id((string)PACKAGE), "Preferences");

    PreferencesBox* prefsBox = manage(new PreferencesBox(prefs));
    prefsBox->destroy.connect(slot(this, &GnoMainWindow::destroyDialog));    
    prefsBox->run();
  }
  

  void* helpProcess(void* location) {
    system(((string*)location)->c_str());
    return (void*)0;
  }


  void* scanProcess(void* newOps) {
    int sourcePort = -1;

    if (((optionsSummary*)newOps)->useSourcePort)
      sourcePort = ((optionsSummary*)newOps)->sourcePort;

    try {
      results = scannerObj.scan(((optionsSummary*)newOps)->start,           // Start port
				((optionsSummary*)newOps)->end,             // End port
				sourcePort,                                 // Source port, -1 if none was specified
				((optionsSummary*)newOps)->extraInfo,       // Extra info?
				((optionsSummary*)newOps)->server,          // Host
				((optionsSummary*)newOps)->netmask,         // Netmask
				((optionsSummary*)newOps)->timeOuts);       // Max time-outs
    }
    catch (scan::DnsError) {
      return((void*)DNS_EXCEPTION);
    }
    catch (scan::SocketFailed) {
      return((void*)SOCKET_FAILED);
    }
    catch (...) {
      return((void*)-1);
    }

    return(void*)0;
  }


  void* scanResultProcess(void*) {
    void* thread_result;

    // See if scanning process finished normally and if errors occured
    if (pthread_join(scanThread, &thread_result) != 0)
      return(thread_result);                // If we reach this point then it means the scan process fucked up
    else {
      if ((int*)thread_result == (int*)DNS_EXCEPTION) {
	gdk_threads_enter();
	Gnome::Dialogs::error("DNS lookup error. Could not resolve host name.");
	// Change GUI
	statusBar->pop(statusBarID);
	statusBar->push(statusBarID, "Ready");
	gdk_threads_leave();
	return thread_result;
      }
      else if ((int*)thread_result == (int*)SOCKET_FAILED) {
	gdk_threads_enter();
	Gnome::Dialogs::error("Networking error. Could not create socket.");
	// Change GUI
	statusBar->pop(statusBarID);
	statusBar->push(statusBarID, "Ready");
	gdk_threads_leave();
	return thread_result;
      }
    }

    //
    // If no errors occured, then go on processing the results:
    //
    // Lock GTK mutex
    gdk_threads_enter();

    // Now process results
    vector<scan::scanResult>::const_iterator curResult = results->begin();
    vector<string> listItems;
    char openPort[32];  // Not nice, but more than large enough...
    string openService;
    
    while (curResult != results->end()) {
      // Convert open ports and services
      snprintf(openPort, 6, "%d", curResult->port);
      openService = curResult->service;
      
      // Now store all open ports, services, etc. and then display
      listItems.push_back(curResult->host);
      listItems.push_back(openPort);
      listItems.push_back(openService);
      listItems.push_back(curResult->info);
      scanCList->rows().push_back(listItems);
      curResult++;
      listItems.clear();
    }

    // Change GUI
    statusBar->pop(statusBarID);
    statusBar->push(statusBarID, "Ready");

    // Release GTK mutex
    gdk_threads_leave();

    return (void*)0;
  }
  
}
