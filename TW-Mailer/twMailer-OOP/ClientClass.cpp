#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "ClientClass.h"

ClientClass::ClientClass()
{

}

ClientClass::~ClientClass()
{

}

int ClientClass::GetClientSocket(){
    return clientSocket;
}

int ClientClass::CreateSocket(){

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1){
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }
    
    return 0;
}

int ClientClass::ConnectSocket(int port,const char* adress){
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);               
    serverAddress.sin_addr.s_addr = inet_addr(adress);

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
        std::cerr << "Error connecting to server" << std::endl;
        return -2;
    }

    return 0;
}