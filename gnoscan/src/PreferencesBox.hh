// PreferencesBox.hh - source file for the GnoScan program
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


#ifndef PREFERENCESBOX_HH
#define PREFERENCESBOX_HH

#include <gtk--/button.h>
#include <gnome--.h>
#include <gnome--/dialog.h>
#include "Preferences.hh"

class PreferencesBox : public Gnome::Dialog {
private:
  // Variables
  pref::Preferences* prefs;
  Gtk::RadioButton* noPort;
  Gtk::RadioButton* aPort;
  Gtk::SpinButton* sourcePortSpin;
  Gtk::CheckButton* extraButton;
  Gtk::SpinButton* maxTimeOuts;

  // Functions
  void init(void);
  void buttonClicked(int);

public:
  PreferencesBox(pref::Preferences*);
  ~PreferencesBox();
};

#endif
