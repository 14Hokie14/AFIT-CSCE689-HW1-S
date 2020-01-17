#pragma once

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/select.h>


#include "Server.h"

class TCPServer : public Server 
{
public:
   TCPServer();
   ~TCPServer();

   // Helper methods for setting up and binding the main server socket:
   void sockSetup(); 
   void sockOptions();
   void sockBind(const char *ip_addr, short unsigned int port);

   // Helper method to send messages on a connected server socket
   void sockSend(int sock, const char* msg);

   // Helper method that just reads in a message and echos the message
   void readEcho();

   // Helper method that checks to see if the passed in buffer says quit
   int quitCheck(const char *buffer);

   // Helper method to 0 out buf
   void zeroBuf();

   // Given methods to implement:
   void bindSvr(const char *ip_addr, unsigned short port);
   void listenSvr();
   void shutdown();

private:
   // Note, I choose to initialize some variables here instead of the constructor, which I'm able to do as of c++11.
   // The difference between doing it here vs in the constructor is when I init something here it happens at compile time,
   // and I'm okay with that happening at compile time because it is such a small amount of code I'm compiling I would 
   // rather keep my constructors clear of initializing variables when possible.  I think on actual production code of a 
   // server you would also want to minimize startup time in this way, but it depends on your use case. 
   int server_sock = 0; 
   int opt = 1; 
   struct sockaddr_in address; 
   char buf[1024] = {0}; // Used to read in client messages, zero it out here
   int valread = 0; // Used to check any read() calls

   std::string initMessage = "====================================================================================\n"
   "Welcome, connection established to the server. Below is a list of possible commands: \n"
   "hello - Displays a greeting\n"
   "1, 2, 3, 4, 5 - Displays unique information to the user\n"
   "passwd - Not currently implemented, will allow the user to change a password\n"
   "exit - Will disconnect the user\n"
   "menu - Will display a list of available commands\n"
   "====================================================================================";

   fd_set master; // Master FD list
   fd_set read_fds; // list used for select()

   int biggestFD = 0; // int used to keep track of the biggest FD yet for the select() call
   int new_sock = 0; // Used when there is a new socket

};


#endif
