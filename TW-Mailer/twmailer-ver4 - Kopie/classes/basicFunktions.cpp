#include "basicFunktions.h"
#include "../headers/PresetsStruct.h"

#include <cmath>
#include <iostream>
#include <sys/socket.h>

basicFunktions::basicFunktions()
{
}

basicFunktions::~basicFunktions()
{
}

std::string basicFunktions::recvFunctBasic(int clientSocket){
    char buffer[1024] = {0};
    int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\0';
    if (errRecv == -1)
    {
        std::cout << "error happened during recv from server" << std::endl;
    }
    return buffer;
}

void basicFunktions::sendFunctBasic(int clientSocket, std::string text){
    send(clientSocket, text.c_str(), text.size() , 0);
}  

int calcPackageNUM(std::string tempString)
{
    if (tempString.size() <= static_cast<long unsigned int>(_blockSIZE)){return 1;}else{
        double rounder = static_cast<double>(tempString.size()) / static_cast<double>(_blockSIZE);
        return std::ceil(rounder);
    }
}

int basicFunktions::sendINFOstring(int clientSocket, TextPreset tp)
{

    send(clientSocket, tp.infoString.c_str(), tp.infoString.size(), 0);
    char buffer[1024] = {0};
    int errRCV = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRCV] = '\0';
    if (errRCV == -1){std::cout << "Error in sendINFOstring" << std::endl;}

    if (std::string(buffer) == "ERR\n"){
        std::cout << "Error in sendINFOstring: not enough elements" << std::endl;
        return -1;
    }
    else{
        std::cout << buffer << std::endl;
    }
    return 0;
}

TextPreset basicFunktions::calcINFOstring(TextPreset tp, int type)
{
    tp.type = type;
    std::string tempString = "";

    switch (tp.type)
    {
    case SEND:
        tempString = tempString + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
        tp.length = tempString.size();
        tp.packageNUM = calcPackageNUM(tempString);
        tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(tp.packageNUM) + "\n" + std::to_string(tp.length) + "\n" + tp.username + "\n";
        break;
    case READ:
        tempString = tempString + tp.username + "\n" + std::to_string(tp.ID) + "\n";
        tp.length = tempString.size();
        tp.packageNUM = calcPackageNUM(tempString);
        tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(tp.packageNUM) + "\n" + std::to_string(tp.length) + "\n" + tp.username + "\n";
        break;
    case QUIT:
        tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(1) + "\n" + std::to_string(1) + "\n" + tp.username + "\n";
        break;
    case LIST:
        tempString = tempString + tp.username + "\n" + std::to_string(tp.ID) + "\n";
        tp.length = tempString.size();
        tp.packageNUM = calcPackageNUM(tempString);
        tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(tp.packageNUM) + "\n" + std::to_string(tp.length) + "\n" + tp.username + "\n";
        break;
    default:
        tempString = tempString + tp.username + "\n" + std::to_string(tp.ID) + "\n";
        tp.length = tempString.size();
        tp.packageNUM = calcPackageNUM(tempString);
        tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(tp.packageNUM) + "\n" + std::to_string(tp.length) + "\n" + tp.username + "\n";
        break;
    }

    return tp;
}
