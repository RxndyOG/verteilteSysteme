#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "txtPreset.h"
#include "ServerClass.h"
#include "BasicSocketFunction.h"

ServerClass::ServerClass()
{

}

ServerClass::~ServerClass()
{

}

int ServerClass::GetServerSocket(){
    return serverSocket;
}

int ServerClass::GetClientSocket(){
    return clientSocket;
}

int ServerClass::CreateSocket(){

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    return serverSocket;
}

int ServerClass::BindSocket(int port){

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // Port number
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);    // Bind to any available interface

    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0){
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
        std::cerr << "Error binding socket" << std::endl;
        return -1;
    }

    return 0;
}

int ServerClass::ListenSockets(char* arg){

    if (listen(serverSocket, 5) == -1){
        std::cerr << "Error listening" << std::endl;
        return -2;
    }

    std::cout << "Server listening on port " << arg << std::endl;
    return 0;
}

int ServerClass::AcceptSocket(){

    clientSize = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientSize);
    if (clientSocket == -1)
    {
        std::cerr << "Error accepting connection" << std::endl;
        return -3;
    }
    return 0;
}

int ServerClass::UserIP(){

    // Get client's IP address and port
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddress.sin_port);
    std::cout << "Accepted connection from " << clientIP << ":" << clientPort << std::endl;

    BasicSocketFunction().sendFunctBasic(clientSocket, "Hello from Server");

    // Send data to the client
    //send(clientSocket, "Hello from Server", sizeof("Hello from Server"), 0);

    return 0;
}

int ServerClass::RecvFromClient(){

    

    return 0;
}