#include "server.h"

void socketHandler:: initServerSocket(){
    if ((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void  socketHandler::bindServer(){
    initServerSocket();
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(masterSocket, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}


void socketHandler::setupClientDescriptors(){
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(masterSocket, &readfds);
    maxSd = masterSocket;

    //add child sockets to set
    for (i = 0; i < maxClients; i++){
        //socket descriptor
        sd = clientSocket[i];

        //if valid socket descriptor then add to read list
        if(sd > 0)
            FD_SET( sd , &readfds);

        //highest file descriptor number, need it for the select function
        if(sd > maxSd)
            maxSd = sd;
    }
}


void socketHandler::startServer(){
    if (listen(masterSocket, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    puts("Waiting for connections ...");
}


socketHandler::socketHandler(){
    maxClients = 30;
    valread = 0;
    PORT = 8888;
    opt = true;

    timeout.tv_sec = timeout.tv_usec = 0;

    memset(clientSocket, 0, sizeof(clientSocket));
    memset(&address, '0', sizeof(address));
    memset(buffer, 0,sizeof(buffer));
    strcpy(message, "Successfully Connected  \r\n");
}




int socketHandler::checkClientActiviy(){
    setupClientDescriptors();

    activity = select( maxSd + 1, &readfds, NULL, NULL, &timeout);

    if ((activity < 0) && (errno!=EINTR)){
        printf("select error\n");
    }

    return activity;
}


void socketHandler::closeSocket(int sd){
    close(sd);
    for (int i = 0; i < maxClients; i++){
        if(clientSocket[i] == sd){
            clientSocket[i] = 0;
        }
    }
}

void socketHandler::stopServer(){
    close(masterSocket);
}

int socketHandler:: handleNewConnection(){
    if ((newSocket = accept(masterSocket,
                             (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
        perror("accept");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < maxClients; i++){
        if (clientSocket[i] == 0){
            clientSocket[i] = newSocket;
            break;
        }
    }
    return newSocket;
}


std::string socketHandler::handleIOActivity(int client_sd){
    if ((valread = read( client_sd, buffer, 1024)) <= 0){
        //Somebody disconnected , get his details and print
        getpeername(client_sd, (struct sockaddr*)&address,(socklen_t*)&addrlen);

        //Close the socket and mark as 0 in list for reuse
        close( client_sd );
        for (int i = 0; i < maxClients; i++){
            if (clientSocket[i] == client_sd)
                clientSocket[i] = 0;
        }
        return "" ;
    }

    else{
        buffer[valread] = '\0';
        std::string message = buffer ; //storing cstring to std::string
        return message ;
    }
}

void socketHandler::sendData(int client_sd , std::string msg){
    send(client_sd, msg.c_str(), msg.length(), 0);
}

std::vector<int> socketHandler::handleActivity(){
    std::vector<int> descriptors;
    if (FD_ISSET(masterSocket, &readfds)){
        descriptors.push_back(-1);
    }

    else{
        //else its some IO operation on some other socket
        for (i = 0; i < maxClients; i++){
            sd = clientSocket[i];

            if (FD_ISSET( sd , &readfds)){
                descriptors.push_back(sd);
            }
        }
    }
    return descriptors;
}