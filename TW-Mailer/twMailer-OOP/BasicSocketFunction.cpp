#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cmath>

#include "ErrorHandling.h"
#include "txtPreset.h"
#include "BasicSocketFunction.h"

BasicSocketFunction::BasicSocketFunction()
{

    argumentParse["SEND"] = [this](txtPreset * tp){ tp->ip.argument = SEND; std::cout << "SEND from Client" << std::endl; return SEND; };
    argumentParse["READ"] = [this](txtPreset * tp){ tp->ip.argument = READ; std::cout << "READ from Client" << std::endl; return READ; };
    argumentParse["LIST"] = [this](txtPreset * tp){ tp->ip.argument = LIST; std::cout << "LIST from Client" << std::endl; return LIST; };
    argumentParse["QUIT"] = [this](txtPreset * tp){ tp->ip.argument = QUIT; std::cout << "QUIT from Client" << std::endl; return QUIT; };
    argumentParse["DEL "] = [this](txtPreset * tp){ tp->ip.argument = DEL; std::cout << "DEL from Client" << std::endl; return DEL; };

}

BasicSocketFunction::~BasicSocketFunction()
{

}

std::string BasicSocketFunction::recvFunctBasic(int socket){

    char buffer[_BlockSize] = {0};
    int errRecv = recv(socket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\n';
    if (errRecv == -1){
        std::cout << "error happened during recv from server" << std::endl;
    }

    return buffer;
}

void BasicSocketFunction::infoStringCalc(txtPreset* tp){
    tp->ip.textLength = tp->sender.size() + tp->subject.size() + tp->text.size();
    tp->ip.packageLength = _BlockSize;
    tp->ip.packageNUM = std::ceil(tp->ip.textLength / tp->ip.packageLength);
    tp->ip.infoString = std::to_string(tp->ip.argument) + "\n" + std::to_string(tp->ip.packageLength) + "\n" + std::to_string(tp->ip.packageNUM) + "\n";
}

int BasicSocketFunction::recvParse(std::string arg){

    txtPreset *tp = new txtPreset;
    auto it = argumentParse.find(arg.substr(0,4));
    if (it != argumentParse.end()) {
        int result = it->second(tp);
        delete tp;
        return result;
    }else{
        std::cout << "COMMENT from Client" << std::endl;
        delete tp;
        return COMMENT;
    }
}

void BasicSocketFunction::sendFunctBasic(int socket, std::string arg){
    send(socket, arg.c_str(), arg.size(), 0);
}
