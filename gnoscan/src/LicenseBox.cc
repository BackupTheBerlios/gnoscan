// LicenseBox.cc - source file for the GnoScan program
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
#include <gnome--.h>
#include "LicenseBox.hh"
#include "config.h"


LicenseBox::~LicenseBox() {
};


LicenseBox::LicenseBox() {
  set_policy(false, false, false);
  set_title ((string)PACKAGE + (string)" License");
  set_border_width(0);
  set_usize(430, 300);
  
  Gtk::Label* label = new Gtk::Label("This program is free software; you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published\nby the Free Software Foundation; either version 2 of the License,\nor (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\nSee the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 59 Temple Place - Suite 330, Boston,\nMA 02111-1307, USA.");

  Gtk::VBox* vbox = get_vbox();
  vbox->pack_start(*label, TRUE, TRUE, 0);
  label->show();

  append_button("OK", GNOME_STOCK_BUTTON_OK);
  set_default(0);
  set_close(true);

  show_all();
}
