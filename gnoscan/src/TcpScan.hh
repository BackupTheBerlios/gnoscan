// TcpScan.hh - source file for the GnoScan program
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

#ifndef TCPSCAN_HH
#define TCPSCAN_HH

#include <string>
#include <vector>
extern "C" {
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
}

namespace scan {

  struct scanResult {
    string host;
    int port;
    string service;
    string info;
  };

  class TcpScan {
  private:
    vector<scanResult> results;

  public:
    TcpScan();
    ~TcpScan();
  
    vector<scanResult>* scan(const int, const int, const int, const bool, const string, const int);
    vector<scanResult>* scanNetmask(const int, const int, const int, const bool, const string, const string, const int);
  };

  void connectAlarm(int);

  // Exceptions
  class DnsError { };
  class SocketFailed { };
  class IOException { };
}

#endif
