// TcpScan.cc - source file for the GnoScan program
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


#include <iostream>
#include <string>
#include <vector>
extern "C" {
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
}
#include "config.h"
#include "TcpScan.hh"


namespace scan {

  TcpScan::TcpScan() {
  }
  
  
  TcpScan::~TcpScan() {
  }
  
  
  vector<scanResult>* TcpScan::scan(int start, int end, string server) {
    timeval timeOut;
    fd_set readSockets;
    int mySocket = 0;
    servent *portname = NULL;
    scanResult result;
    sockaddr_in dest;
    string openService, extraInfo;
    hostent *myHost = NULL;
    char myBuffer[512];

    // Clear previous results
    results.clear();

    // Scan ports in range
    for (int i = start; i <= end; i++) {
      FD_ZERO(&readSockets);
      timeOut.tv_sec = 2;
      timeOut.tv_usec = 0;
      
      if ( (mySocket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
	cerr << (string)PACKAGE << ": Socket for port " << i << " failed to open." << endl;
	close(mySocket);
	break;
      }
      else
	myHost = gethostbyname(server.c_str());

      if (!myHost)
	throw DnsError();
      else {
	try {
	  // Set communication parameters
	  memset((char*)&dest, 0, sizeof(sockaddr_in));
	  dest.sin_port = htons(i);
	  dest.sin_family = PF_INET;
	  memcpy(&(dest.sin_addr.s_addr), myHost->h_addr, myHost->h_length);
	  
	  // Try to open the port
	  if (connect(mySocket, (const sockaddr*)&dest, sizeof(dest)) == 0) {
	    FD_SET(mySocket, &readSockets);                                         // Put current socket into the list
	    int z = select (mySocket + 1, &readSockets, NULL, NULL, &timeOut);      // Wait for timeout

	    if (z > 0) {
	      z = read(mySocket, &myBuffer, sizeof myBuffer - 1);
	      
	      if (z > 0) {
		myBuffer[z] = 0;
		extraInfo = myBuffer;
	      }
	      else
		extraInfo = "not specified";
	    }
	    else
	      extraInfo = "not specified";

	    FD_ZERO(&readSockets);
	    portname = getservbyport(htons(i), "tcp");
	    
	    if (portname) {
	      result.host = server;
	      openService = portname->s_name;
	      result.service = openService;
	      result.port = i;
	      result.info = extraInfo;
	    }
	    else {
	      result.host = server;
	      result.service = "unspecified";
	      result.port = i;
	      result.info = extraInfo;
	    }

	    results.push_back(result);

	    // Close connection
	    if (close(mySocket) == -1)
	      throw CloseException();

	    // Clean-up
	    portname = NULL;
	    myHost = NULL;
	  }
	}
	catch (...) {
	  throw;
	}
      }
    }

    return &results;
  }

}
