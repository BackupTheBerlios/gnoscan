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
#include <errno.h>
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
#include <string.h>
#include <signal.h>
}
#include "config.h"
#include "TcpScan.hh"


namespace scan {

  static int timeouts;

  TcpScan::TcpScan() {
    timeouts = 0;
  }
  
  
  TcpScan::~TcpScan() {
  }
  
  
  // The main scanning function
  vector<scanResult>* TcpScan::scan(const int start,
				    const int end,
				    const int sourcePort,
				    const bool info,
				    const string newServer,
				    const int maxTimeOuts) {
    timeval timeOut;
    fd_set readSockets;
    servent *portname = NULL;
    scanResult result;
    sockaddr_in dest, source;
    hostent *myHost = NULL;
    struct sigaction sigact;
    string openService, extraInfo, server = newServer;
    char myBuffer[512];
    int mySocket = 0, z = 0;

    // Clear previous results
    results.clear();

    // Scan ports in range
    for (int i = start; i <= end; i++) {
      FD_ZERO(&readSockets);
      timeOut.tv_sec = 2;
      timeOut.tv_usec = 0;
      
      if ( (mySocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	cerr << (string)PACKAGE << ": Socket for port " << i << " failed to open." << endl;
	cerr << (string)PACKAGE << ": Error number " << errno << " occured: " << strerror(errno) << "." << endl;
	throw SocketFailed();
	break;
      }
      else {
	// Try to resolve host name
	myHost = gethostbyname(server.c_str());
      }
      
      if (!myHost)
	throw DnsError();
      else {
	try {
	  // Set communication parameters
	  memset((char*)&dest, 0, sizeof(sockaddr_in));
	  dest.sin_port = htons(i);
	  dest.sin_family = AF_INET;
	  memcpy(&(dest.sin_addr.s_addr), myHost->h_addr, myHost->h_length);
	  memset((char*)&source, 0, sizeof(sockaddr_in));
	  source.sin_port = htons(0);
	  source.sin_family = AF_INET;
	  source.sin_addr.s_addr = htonl(INADDR_ANY);
	  
	  // Specifiy source port if given
	  if (sourcePort != -1) {
	    source.sin_port = htons(sourcePort);
	    
	    do {
	      z = bind(mySocket, (const sockaddr*)&source, sizeof(source));
	    } while(z);
	  }
	  
	  // If too many timeouts occur, assume the host is not reachable and quit scanning process
	  if (timeouts >= maxTimeOuts) {
	    timeouts = 0;
	    break;
	  }
	  
	  // Use alarm to time out after 3 seconds
	  sigact.sa_handler = connectAlarm;
	  sigemptyset(&sigact.sa_mask);
	  sigact.sa_flags = 0;
	  
	  if (sigaction(SIGALRM, &sigact, NULL) < 0) {
	    cerr << (string)PACKAGE << ": Error: Problems with alarm signal. Please report a bug." << endl;
	    throw IOException();
	  }
	  
	  alarm(3);
	  
	  // Try to open the port
	  if (connect(mySocket, (const sockaddr*)&dest, sizeof(dest)) == 0) {
	    
	    // Try to get additional information on the open port
	    if (info) {
	      FD_SET(mySocket, &readSockets);
	      z = select(mySocket + 1, &readSockets, NULL, NULL, &timeOut);
	      
	      if (z > 0) {
		z = read(mySocket, &myBuffer, sizeof(myBuffer) - 5);
		
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
	    }
	    else
	      extraInfo = "";
	    
	    // Store the results
	    portname = getservbyport(htons(i), "tcp");
	    server = (string)(inet_ntoa(dest.sin_addr));
	    
	    if (portname) {
	      result.host = server;
	      openService = portname->s_name;
	      result.service = openService;
	      result.port = i;
	      result.info = extraInfo;
	    }
	    else {
	      result.host = server;
	      result.service = "unknown";
	      result.port = i;
	      result.info = extraInfo;
	    }
	    
	    results.push_back(result);
	  }
	  
	  // Close connection, regardless of errors,
	  // cause most sockets are created without actually getting connected to ports on the host anyway
	  shutdown(mySocket, 2);
	  close(mySocket);
	  
	  // Clean-up
	  alarm(0);
	  portname = NULL;
	  myHost = NULL;
	  mySocket = 0;
	}
	catch (...) {
	  shutdown(mySocket, 2);
	  close(mySocket);
	  throw;
	}
      }
    }
    
    return &results;
  }
  

  // The main scanning function for netmasks
  vector<scanResult>* TcpScan::scanNetmask(const int start,
					   const int end,
					   const int sourcePort,
					   const bool info,
					   const string newServer,
					   const string sNetmask,
					   const int maxTimeOuts) {
    timeval timeOut;
    fd_set readSockets;
    servent *portname = NULL;
    scanResult result;
    sockaddr_in dest, source;
    hostent *myHost = NULL;
    struct sigaction sigact;
    string openService, extraInfo, server = newServer;
    char myBuffer[512];
    int mySocket = 0, z = 0, bits = 0, failures = 0;
    unsigned long netmask = 0, curIP = 0, startHost = 0, beginIP = 0, endIP = 0;

    netmask = inet_addr(sNetmask.c_str());
    for (curIP = ntohl(netmask)^0xFFFFFFFF, bits = 0; curIP; curIP = (curIP >> 1), bits++);
    bits = 32 - bits;
    
    // Convert specified host string into internal binary format
    if ((startHost = inet_addr(server.c_str())) == INADDR_NONE) {
      if (!(myHost = gethostbyname(server.c_str())))
	throw DnsError();
      else {
	startHost = ((in_addr*)myHost->h_addr)->s_addr;
	server = (string)(inet_ntoa(*((in_addr*)myHost->h_addr)));
      }
    }
    
    // Specify start and end IP addresses of subnet
    beginIP = ntohl(netmask & startHost) + 1;
    endIP = beginIP;
    for (int y = 0; y < 32 - bits; y++)
      endIP += (1 << y);
    
    // Clear previous results
    results.clear();

    // Scan all hosts in given netmask range
    for (curIP = beginIP; curIP <= endIP; curIP++) {
      dest.sin_addr.s_addr = htonl(curIP);

      // Scan ports in range
      for (int i = start; i <= end; i++) {
	FD_ZERO(&readSockets);
	timeOut.tv_sec = 2;
	timeOut.tv_usec = 0;
	
	if ( (mySocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	  cerr << (string)PACKAGE << ": Socket for port " << i << " failed to open." << endl;
	  cerr << (string)PACKAGE << ": Error number " << errno << " occured: " << strerror(errno) << "." << endl;
	  throw SocketFailed();
	  break;
	}

	try {
	  // Set communication parameters
	  memset((char*)&dest, 0, sizeof(sockaddr_in));
	  dest.sin_port = htons(i);
	  dest.sin_family = AF_INET;
	  dest.sin_addr.s_addr = htonl(curIP);
	  
	  memset((char*)&source, 0, sizeof(sockaddr_in));
	  source.sin_port = htons(0);
	  source.sin_family = AF_INET;
	  source.sin_addr.s_addr = htonl(INADDR_ANY);
	  
	  // Specifiy source port if given
	  if (sourcePort != -1) {
	    source.sin_port = htons(sourcePort);
	    
	    do {
	      z = bind(mySocket, (const sockaddr*)&source, sizeof(source));
	    } while(z);
	  }
	  
	  // If too many timeouts occur, assume the host is not reachable and quit scanning process
	  if (timeouts >= maxTimeOuts) {
	    timeouts = 0;
	    failures++;
	    break;
	  }
	  
	  // Use alarm to time out after 3 seconds
	  sigact.sa_handler = connectAlarm;
	  sigemptyset(&sigact.sa_mask);
	  sigact.sa_flags = 0;
	  
	  if (sigaction(SIGALRM, &sigact, NULL) < 0) {
	    cerr << (string)PACKAGE << ": Error: Problems with alarm signal. Please report a bug." << endl;
	    throw IOException();
	  }
	  
	  alarm(3);
	  
	  // Try to open the port
	  if (connect(mySocket, (const sockaddr*)&dest, sizeof(dest)) == 0) {
	    
	    // Try to get additional information on the open port
	    if (info) {
	      FD_SET(mySocket, &readSockets);
	      z = select(mySocket + 1, &readSockets, NULL, NULL, &timeOut);
	      
	      if (z > 0) {
		z = read(mySocket, &myBuffer, sizeof(myBuffer) - 5);
		
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
	    }
	    else
	      extraInfo = "";
	    
	    // Store the results
	    portname = getservbyport(htons(i), "tcp");
	    server = (string)(inet_ntoa(dest.sin_addr));
	    
	    if (portname) {
	      result.host = server;
	      openService = portname->s_name;
	      result.service = openService;
	      result.port = i;
	      result.info = extraInfo;
	    }
	    else {
	      result.host = server;
	      result.service = "unknown";
	      result.port = i;
	      result.info = extraInfo;
	    }
	    
	    results.push_back(result);
	  }
	  
	  // Close connection, regardless of errors,
	  // cause most sockets are created without actually getting connected to ports on the host anyway
	  shutdown(mySocket, 2);
	  close(mySocket);
	  
	  // Clean-up
	  alarm(0);
	  portname = NULL;
	  myHost = NULL;
	  mySocket = 0;
	  
	  // If too many failures happen, cancel scan process
	  if (failures >= maxTimeOuts)
	    return &results;
	}
	catch (...) {
	  shutdown(mySocket, 2);
	  close(mySocket);
	  throw;
	}
      }
    }
    
    return &results;
  }


  void connectAlarm(int signo) {
    timeouts++;
    return;
  }

}
