#include "server.h"

void socketHandler:: initServerSocket(){
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void  socketHandler::bindServer(){
    initServerSocket();
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}


void socketHandler::setupClientDescriptors(){
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    //add child sockets to set
    for (i = 0; i < max_clients; i++){
        //socket descriptor
        sd = client_socket[i];

        //if valid socket descriptor then add to read list
        if(sd > 0)
            FD_SET( sd , &readfds);

        //highest file descriptor number, need it for the select function
        if(sd > max_sd)
            max_sd = sd;
    }
}


void socketHandler::startServer(){
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    puts("Waiting for connections ...");
}


//initialise all client_socket[] to 0 so not checked
socketHandler::socketHandler(){
    max_clients = 30;
    valread = 0;
    PORT = 8888;
    opt = true;

    timeout.tv_sec = timeout.tv_usec = 0;

    //initialise all client_socket[] to 0 so not checked
    memset(client_socket, 0, sizeof(client_socket));
    memset(&address, '0', sizeof(address));
    memset(buffer, 0,sizeof(buffer));
    strcpy(message, "Successfully Connected  \r\n");
}




int socketHandler::checkClientActiviy(){
    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely
    setupClientDescriptors() ;

    activity = select( max_sd + 1 , &readfds , NULL , NULL , &timeout);

    if ((activity < 0) && (errno!=EINTR)){
        printf("select error\n");
    }

    return activity ;
}


void socketHandler::closeSocket(int sd){
    close(sd) ;
    for(int i=0 ; i<max_clients ; i++){
        if(client_socket[i]==sd){
            client_socket[i] = 0 ;
        }
    }
}

void socketHandler::stopServer(){
    close(master_socket) ;
}


//returns the sd of new client
int socketHandler:: handleNewConnection(){
    if ((new_socket = accept(master_socket,
                             (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
        perror("accept");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < max_clients; i++){
        //if position is empty
        if( client_socket[i] == 0 ){
            client_socket[i] = new_socket;
            printf("Adding to list of sockets as %d\n" , i);

            break;
        }
    }

    return new_socket ;
}


std::string socketHandler::handleIOActivity(int client_sd){
    if ((valread = read( client_sd , buffer, 1024)) <= 0){
        //Somebody disconnected , get his details and print
        getpeername(client_sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);
        // printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

        //Close the socket and mark as 0 in list for reuse
        close( client_sd );
        for(int i =0 ; i<max_clients ; i++){
            if(client_socket[i]==client_sd)
                client_socket[i] = 0;
        }

        return "" ;
    }

        //Echo back the message that came in
    else{
        //set the string terminating NULL byte on the end
        //of the data read
        buffer[valread] = '\0';
        // std::cout<< "\nMessage from client : " << buffer << " ; Length : " <<valread ;
        std::string message = buffer ; //storing cstring to std::string
        return message ;

        // send(client_sd , buffer , strlen(buffer) , 0 );
    }
}

void socketHandler::sendData(int client_sd , std::string msg)
{
    send(client_sd , msg.c_str() , msg.length() , 0) ;
}


//Returns array with -1 if its a new connection else returns a list of client sds
std::vector<int> socketHandler::handleActivity(){
    std::vector <int> descriptors ;
    //If something happened on the master socket ,
    //then its an incoming connection
    if (FD_ISSET(master_socket, &readfds)){
        descriptors.push_back(-1);
    }

    else{
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++){
            sd = client_socket[i];

            if (FD_ISSET( sd , &readfds)){
                descriptors.push_back(sd) ;
            }
        }
    }

    return descriptors ;
}