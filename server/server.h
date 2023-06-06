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
    int masterSocket, PORT, addrlen, newSocket, clientSocket[30],
          maxClients, activity, i, valread, sd;
    int maxSd;
    struct sockaddr_in address;
    char message[50];
    struct timeval timeout;
        
    char buffer[1025];  //data buffer of 1K 
    fd_set readfds;

    void initServerSocket();

public:
    socketHandler();
    void bindServer();
    void sendData(int client_sd, const std::vector<char>& msg);
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
