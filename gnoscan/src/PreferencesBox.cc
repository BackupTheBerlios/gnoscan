// PreferencesBox.cc - source file for the GnoScan program
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


#include <gtk--/box.h>
#include <gtk--/label.h>
#include <gtk--/button.h>
#include <gtk--/frame.h>
#include <gtk--/separator.h>
#include <gtk--/entry.h>
#include <gnome--.h>
#include "GnoScan.hh"
#include "GnoMainWindow.hh"
#include "PreferencesBox.hh"


PreferencesBox::~PreferencesBox() {
};


PreferencesBox::PreferencesBox() {
  set_policy(false, false, false);
  set_title ("Options");
  set_border_width(0);
  set_usize(320, 200);
  init();
}


void PreferencesBox::init(void) {
  Gtk::HBox* sourceHBox = manage(new Gtk::HBox());
  Gtk::VBox* sourceVBox = manage(new Gtk::VBox());
  Gtk::HBox* aPortHBox = manage(new Gtk::HBox());
  Gtk::Frame* sourceFrame = manage(new Gtk::Frame("Source Port:"));
  Gtk::RadioButton_Helpers::Group gr;
  Gtk::RadioButton* noPort = manage(new Gtk::RadioButton(gr, "No specific port (recommended, MUCH faster)", 0));
  Gtk::RadioButton* aPort = manage(new Gtk::RadioButton(gr, "Use a specific port:", 0));
  Gtk::Adjustment* spinbutton_adj = manage(new Gtk::Adjustment(0, 0, 10000, 1, 10, 10));
  Gtk::SpinButton* sourcePortSpin = manage(new Gtk::SpinButton(*spinbutton_adj, 1, 0));
  Gtk::CheckButton* extraButton = manage(new Gtk::CheckButton("Display extra information", 0.0, 0.5));
  aPortHBox->pack_start(*aPort, FALSE, FALSE, 0);
  aPortHBox->pack_start(*sourcePortSpin, FALSE, FALSE, 0);
  sourceVBox->pack_start(*noPort);
  sourceVBox->pack_start(*aPortHBox);
  sourceHBox->pack_start(*sourceVBox, TRUE, TRUE, STD_PADDING);
  sourceFrame->add(*sourceHBox);

  // Put together and show the dialog controls
  Gtk::VBox* mainVBox = get_vbox();
  mainVBox->pack_start(*sourceFrame);
  mainVBox->pack_start(*extraButton);
  
  // Add buttons
  append_button("OK", GNOME_STOCK_BUTTON_OK);
  append_button("Cancel", GNOME_STOCK_BUTTON_CANCEL);
  set_default(0);
  set_close(true);
  show_all();
}
