#include <stdio.h>
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include<iostream>
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include<bits/stdc++.h>
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <ncurses.h>

class socketHandler{
    int opt;
    int master_socket, PORT, addrlen, new_socket, client_socket[30],
          max_clients, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    char message[50];
    struct timeval timeout;
        
    char buffer[1025];  //data buffer of 1K 
    fd_set readfds;

    void initServerSocket() ;

public:
    socketHandler();
    void bindServer();
    std::vector<int> handleActivity();
    void setupClientDescriptors();
    void startServer();
    void stopServer();
    void sendData(int, std::string);
    int checkClientActiviy();
    void closeSocket(int);
    int handleNewConnection();
    std::string handleIOActivity(int);
};
