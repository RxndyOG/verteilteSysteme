#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "BasicSocketFunction.h"
#include "ErrorHandling.h"
#include "txtPreset.h"

BasicSocketFunction::BasicSocketFunction()
{

}

BasicSocketFunction::~BasicSocketFunction()
{

}

std::string BasicSocketFunction::recvFunctBasic(int socket){

    char buffer[1024] = {0};
    int errRecv = recv(socket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\n';
    if (errRecv == -1){
        std::cout << "error happened during recv from server" << std::endl;
    }

    return buffer;
}

int BasicSocketFunction::recvParse(std::string arg){
    
    if(arg.substr(0,4) == "SEND"){
        std::cout << "SEND from Client" << std::endl;
        return SEND;
    }
    if(arg.substr(0,4) == "READ"){
        std::cout << "READ from Client" << std::endl;
        return READ;
    }
    if(arg.substr(0,4) == "LIST"){
        std::cout << "LIST from Client" << std::endl;
        return LIST;
    }
    if(arg.substr(0,3) == "DEL"){
        std::cout << "DEL from Client" << std::endl;
        return DEL;
    }
    if(arg.substr(0,4) == "QUIT"){
        std::cout << "QUIT from Client" << std::endl;
        return QUIT;
    }
    std::cout << "CLIENT COMMENT: " << arg << std::endl;
    return COMMENT;
}

void BasicSocketFunction::sendFunctBasic(int socket, std::string arg){
    send(socket, arg.c_str(), arg.size(), 0);
}
