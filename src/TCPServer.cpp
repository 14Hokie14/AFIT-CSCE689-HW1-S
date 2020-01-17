#include "TCPServer.h"

/**
 * I am using the following sites as references to create this file and the rest of my code base: 
 * https://www.geeksforgeeks.org/socket-programming-cc/
 * https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
 * https://www.geeksforgeeks.org/tcp-and-udp-server-using-select/
 * https://stackoverflow.com/questions/15673846/how-to-give-to-a-client-specific-ip-address-in-c
 * http://beej.us/guide/bgnet/html/  // This document is amazing. 
 *
 * Author: Joshua H. White
**/

/**
* Constructor, does nothing for now. Also calls the constructor for Server. 
**/
TCPServer::TCPServer() : Server() {}

/**
* Destructor, does nothing for now. 
**/
TCPServer::~TCPServer() {}


/**
* Helper methods for the server:
**/

/**
* Helper method to set up the socket for the server. 
**/ 
void TCPServer::sockSetup(){
    this->server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if( this->server_sock == 0 ){
        perror("Failure in socket() in TCPServer.\n");
        exit(EXIT_FAILURE);
    }
    this->biggestFD = this->server_sock;
}


/**
* Helper method that wraps the setsockopt call. 
*
* Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
**/
void TCPServer::sockOptions(){
    if ( setsockopt(this->server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &this->opt, sizeof(this->opt)) ){
        perror("Failure in setsockopt() in TCPServer.\n");
        exit(EXIT_FAILURE);
    }
}


/**
* Helper method that wraps the bind call, and any required setup. 
*
* Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
**/
void TCPServer::sockBind(const char *ip_addr, short unsigned int port){
    // Set up the sockaddr_in struct here:
    this->address.sin_family = AF_INET; 
    // this->address.sin_addr.s_addr = INADDR_ANY; // Won't use this, this uses the computer's default IP of 127.0.0.1
    inet_pton(AF_INET, ip_addr, &(this->address.sin_addr)); // Inserts the IP address as a string into the sockaddr_in struct
    this->address.sin_port = htons( port ); // htons is shorthand for Host-to-Network short
    
    //std::cout << "here" << std::endl; 
    
    // Now bind the socket, if it fails print an error message and exit
    if ( ( bind(this->server_sock, (struct sockaddr *)&this->address, sizeof(this->address)) ) < 0 ){
        perror("Failure in bind() in TCPServer.\n");
        exit(EXIT_FAILURE);
    }
}


/**
* Helper method to send messages on a connected server socket
**/
   void TCPServer::sockSend(int sock, const char* msg){
       send(sock, msg, strlen(msg), 0);
}


/**
* Helper method that just reads in a message and echos the message
**/
void TCPServer::readEcho(){
    zeroBuf();
    valread = read (new_sock, buf, 1024);
    std::cout << buf << std::endl;
    buf[valread] = '\0';
    sockSend(new_sock, &buf[0]);
}


/**
* Helper method that checks to see if the passed in buffer says quit
*
* Returns: 0 if the parameter is "quit", -1 otherwise
**/
int TCPServer::quitCheck(const char *buffer){
    std::string q = "quit";
    if(q.compare(buffer) == 0){
        return 0; 
    }
    return -1; 
}


/**
* Helper method to 0 out buf
**/
void TCPServer::zeroBuf(){
    for(int i = 0; i < 1024; i++){
        this->buf[i] = '\0';
    }
}

//============================================================================================================================================================================
/** 
* Given methods to implement below: 
**/


/**********************************************************************************************
 * bindSvr - Creates a network socket and sets it nonblocking so we can loop through looking for
 *           data. Then binds it to the ip address and port
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
**********************************************************************************************/
void TCPServer::bindSvr(const char *ip_addr, short unsigned int port) {
    // Socket setup:
    sockSetup();

    // Set up the options of the socket: 
   sockOptions();

    // Bind the socket:
    sockBind(ip_addr, port);
}


/**********************************************************************************************
 * listenSvr - Performs a loop to look for connections and create TCPConn objects to handle
 *             them. Also loops through the list of connections and handles data received and
 *             sending of data.   
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
**********************************************************************************************/
void TCPServer::listenSvr() {
    

    // Intermediate step, we are just going to make an echo server. 
    if( listen(this->server_sock, 10) < 0){
        perror("Failure in listen() in TCPServer.\n");
        exit(EXIT_FAILURE);
    }

    // Call accept, if it fails print an error message and exit
    int addrlen = sizeof(this->address);
    new_sock = accept(this->server_sock, (struct sockaddr *)&this->address, (socklen_t*)&addrlen);
    if ( new_sock < 0){
        perror("Failure in accept() in TCPServer.\n");
        exit(EXIT_FAILURE);
    }

    //send(new_sock, initMessage.c_str(), strlen(initMessage.c_str()), 0); 
    sockSend(new_sock, initMessage.c_str());
    std::cout << "Sent welcome message to client socket: " << new_sock << std::endl; 

    while(true){
        readEcho();
        if(strcmp(buf, "quit") == 0){ 
            break;
        }
    }
    
}

/**********************************************************************************************
 * shutdown - Cleanly closes the socket FD.
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::shutdown() {
    if( close(server_sock) ){
        perror("Failure in close() in TCPServer.");
        exit(EXIT_FAILURE);
    }
}
