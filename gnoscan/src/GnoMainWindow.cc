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
#include <gtk--/frame.h>
#include <gtk--/separator.h>
#include <gtk--/spinbutton.h>
#include <gtk--/adjustment.h>
#include <gtk--/scrolledwindow.h>
#include <gtk--/statusbar.h>
#include <gtk--/clist.h>
#include <gtk--/base.h>
#include <gnome--.h>
#include <gnome--/main.h>
#include <gnome--/app.h>
#include <gnome--/about.h>
#include <gnome--/dialog.h>
#include "config.h"
#include "GnoMainWindow.hh"
#include "TcpScan.hh"


using SigC::slot;
using SigC::bind;

namespace gnomain {

  GnoMainWindow::GnoMainWindow(string name): Gnome::App(name, name)
  {
    aboutBox = NULL;
    licenseBox = NULL;

    try {
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
    scanOptions options;

    set_policy(false, true, false);
    set_default_size(appWidth, appHeight);

    try {
      // Server entry-box and label
      Gtk::HBox* serverHBox = manage(new Gtk::HBox());
      Gtk::Label* serverLabel = manage(new Gtk::Label("Host:"));
      Gtk::Entry* serverEntry = manage(new Gtk::Entry(40));
      serverEntry->set_text("192.168.1.2");
      serverHBox->pack_start(*serverLabel, FALSE, FALSE, STD_PADDING);
      serverHBox->pack_start(*serverEntry, TRUE, TRUE, STD_PADDING);
      
      // Port range spin buttons and frame
      Gtk::HBox* portHBox = manage(new Gtk::HBox());
      Gtk::VBox* portVBox = manage(new Gtk::VBox());
      Gtk::HBox* portButtonsHBox = manage(new Gtk::HBox(TRUE, 0));
      Gtk::Frame* portRangeFrame = manage(new Gtk::Frame("Port Range:"));
      Gtk::Label* portStartLabel = manage(new Gtk::Label("Start:"));
      Gtk::Adjustment* spinbutton1_adj = manage(new Gtk::Adjustment(0, 0, 10000, 1, 10, 10));
      Gtk::SpinButton* portStart = manage(new Gtk::SpinButton(*spinbutton1_adj, 1, 0));
      Gtk::Label* portEndLabel = manage(new Gtk::Label("End:"));
      Gtk::Adjustment* spinbutton2_adj = manage(new Gtk::Adjustment(1023, 0, 10000, 1, 10, 10));
      Gtk::SpinButton* portEnd = manage(new Gtk::SpinButton(*spinbutton2_adj, 1, 0));
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
      Gtk::HButtonBox* confirmButtonBox = manage(new Gtk::HButtonBox());
      confirmButtonBox->set_border_width(STD_PADDING_HALF);
      confirmButtonBox->set_spacing(STD_PADDING);
      confirmButtonBox->set_layout(GTK_BUTTONBOX_END);
      Gtk::Separator* sep = manage(new Gtk::HSeparator()); // h-separator
      Gtk::Button* cancelButton = manage(new Gnome::StockButton(GNOME_STOCK_BUTTON_CANCEL));
      Gtk::Button* scanButton = manage(new Gtk::Button("Scan"));
      confirmButtonBox->add(*scanButton);
      confirmButtonBox->add(*cancelButton);
      
      // Status bar
      statusBar = new Gtk::Statusbar();
      statusBar->push(statusBar->get_context_id((string)PACKAGE), "Ready");

      // Draw and arrange all widgets
      Gtk::VBox* vBox = manage(new Gtk::VBox());
      vBox->pack_start(*serverHBox, TRUE, FALSE, STD_PADDING);  // EXPAND, FILL, PADDING
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
      
      // Set button-defaults and connect signals
      scanButton->set_flags(GTK_CAN_DEFAULT);
      cancelButton->set_flags(GTK_CAN_DEFAULT);
      scanButton->clicked.connect(bind(slot(this, &GnoMainWindow::startScan), options));
      cancelButton->clicked.connect(slot(this, &GnoMainWindow::closeWindow));
      scanButton->grab_default();
      
      // Connect destroy handler
      destroy.connect(slot(this, &GnoMainWindow::closeWindow));
    }
    catch (...) {
      throw;
    }
  }
  
  
  void GnoMainWindow::installMenus(void) {
    // Help menu
    menuHelp.push_back(Gnome::UI::Item("Help..."));
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
    menuFile.push_back(Gnome::MenuItems::Exit(slot(this, &GnoMainWindow::closeWindow)));
    
    // Menu bar
    menuBar.push_back(Gnome::Menus::File(menuFile));
    menuBar.push_back(Gnome::Menus::Edit(menuEdit));
    menuBar.push_back(Gnome::Menus::Help(menuHelp));
    
    // Draw handle box with menu bar and menu items
    create_menus(menuBar);
  }
  
  
  void GnoMainWindow::startScan(scanOptions ops) {
    string serverName = ops.server->get_text();

    // Check if server was specified, etc.
    if (!serverName.length()) {
      Gnome::Dialogs::error("Please specifiy the server first.");
      return;
    }
    
    if (ops.start->get_value_as_int() > ops.end->get_value_as_int()) {
      Gnome::Dialogs::error("Start port must be equal or greater than end port.");
      return;
    }

    // Now begin the scanning process...
    scanCList->rows().clear();
    statusBar->pop(statusBar->get_context_id((string)PACKAGE));
    statusBar->push(statusBar->get_context_id((string)PACKAGE), "Scanning...");

    try {
      const vector<scan::scanResult>* results = scannerObj.scan(ops.start->get_value_as_int(), ops.end->get_value_as_int(), ops.server->get_text());
      vector<scan::scanResult>::const_iterator curResult = results->begin();
      vector<string> listItems;
      char openPort[32];  // Not nice but more than large enough...
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
    }
    catch (scan::DnsError) {
      Gnome::Dialogs::error("DNS lookup error. Could not resolve domain name.");
    }
    catch (...) {
      throw;
    }

    // Reset status bar
    statusBar->pop(statusBar->get_context_id((string)PACKAGE));
    statusBar->push(statusBar->get_context_id((string)PACKAGE), "Ready");
  }

  
  void GnoMainWindow::closeWindow(void) {
    Gnome::Main::quit();
  }
  
  
  void GnoMainWindow::displayAboutBox(void) {
    statusBar->pop(statusBar->get_context_id((string)PACKAGE));
    statusBar->push(statusBar->get_context_id((string)PACKAGE), "About GnoScan");

    if (aboutBox != NULL) {
      Gdk_Window aboutWindow(aboutBox->get_window());
      aboutWindow.show();
      aboutWindow.raise();
    }
    else {
      vector<string> authors;
      string copyright =   "Copyright (c) 2001 Andreas Bauer";
      string author =      "Andreas Bauer <baueran@users.berlios.de>";
      string description = "Use GnoScan to scan and secure your network.\nSee http://gnoscan.berlios.de/ for further information.";
      
      authors.push_back(author);
      aboutBox = manage(new Gnome::About((string)PACKAGE, (string)VERSION, copyright, authors, description, 0));
      aboutBox->set_parent(*this);
      aboutBox->destroy.connect(slot(this,&GnoMainWindow::destroyAboutBox));
      aboutBox->show();
    }
  }
  
  
  void GnoMainWindow::destroyAboutBox(void) {
    aboutBox = NULL;
    statusBar->pop(statusBar->get_context_id((string)PACKAGE));
    statusBar->push(statusBar->get_context_id((string)PACKAGE), "Ready");
  }
  
  
  void GnoMainWindow::displayLicenseBox(void) {
    statusBar->pop(statusBar->get_context_id((string)PACKAGE));
    statusBar->push(statusBar->get_context_id((string)PACKAGE), "GnoScan License");

    if (licenseBox != NULL) {
      Gdk_Window licenseWindow(licenseBox->get_window());
      licenseWindow.show();
      licenseWindow.raise();
    }
    else {
      licenseBox = manage(new LicenseBox());
      licenseBox->destroy.connect(slot(this,&GnoMainWindow::destroyLicenseBox));
      licenseBox->show();
    }
  }
  

  void GnoMainWindow::destroyLicenseBox(void) {
    licenseBox = NULL;
    statusBar->pop(statusBar->get_context_id((string)PACKAGE));
    statusBar->push(statusBar->get_context_id((string)PACKAGE), "Ready");
  }
  

  void GnoMainWindow::displayOptions() {
    
  }
  
}
