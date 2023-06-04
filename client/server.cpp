#include "server.h"

SocketHandler::SocketHandler(){
    sock = 0;
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(buffer, 0,sizeof(buffer));
    timeout.tv_sec = timeout.tv_usec = 0;
}

void SocketHandler::sendData(string message){
    send(sock, message.c_str(), message.length(), 0);
}

//creates and assigns the fd value to sock member
void SocketHandler::createClientSocket(){
    int opt= 1;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    FD_ZERO(&set); /* clear the set */
    FD_SET(sock, &set); /* add our file descriptor to the set */

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void SocketHandler::initServerAddress(string address, int port){
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET,address.c_str(), &serv_addr.sin_addr) <= 0){
        printf("\nInvalid address/ Address not supported \n");
    }
}


int SocketHandler::connectToServer(string address, int port) {
    createClientSocket();
    initServerAddress(address, port);
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        close(sock);
        return 2;
    }
    int connectResult = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (connectResult < 0) {
        printf("\nConnection Failed \n");
        close(sock);
        return 2;
    }
    return 0;
}

void SocketHandler::closeSocket(){
    close(sock);
}

int SocketHandler::getSock() const{
    return sock;
}

char* SocketHandler::getBuffer(){
    return buffer;
}

fd_set& SocketHandler::getSet(){
    return set;
}

timeval& SocketHandler::getTimeout(){
    return timeout;
}