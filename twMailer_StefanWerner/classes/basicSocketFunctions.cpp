#include "basicSocketFunctions.h"
#include "../headers/PresetStruct.h"

#include <cmath>
#include <sys/socket.h>
#include <iostream>
#include <cstring>

basicSocketFunctions::basicSocketFunctions()
{

}

basicSocketFunctions::~basicSocketFunctions()
{

}

recvReturn basicSocketFunctions::recvFunctBasic(int client_socket)        // gets the send() from server and parses them into buffer and err for error handeling
{
    recvReturn rr;
    char buffer[1024] = {};
    memset(buffer, 0, sizeof(buffer));
    int err = recv(client_socket, buffer, sizeof(buffer), 0);
    if (err == -1)
    {
        rr.buffer = "ERR";
        rr.err = err;
        return rr;
    }
    buffer[err] = '\0';
    rr.buffer = buffer;
    rr.err = err;
    return rr;
}

INFOpreset basicSocketFunctions::SEND_and_CALC_infoString(TEXTpreset tp, int socked_fd)   // sends the info string to server for parse of requirted funktion (SEND, READ, LIST etc...)
{
    std::string infoString = "";
    INFOpreset ip;
    ip.textLength = tp.receiver.size() + 1 + tp.subject.size() + 1 + tp.message.size() + 1;
    float result = static_cast<float>(ip.textLength) / (_BLOCK_SIZE - 1);
    ip.numPack = std::ceil(result);
    infoString = std::to_string(ip.textLength) + "\n" + std::to_string(ip.numPack) + "\n";
    send(socked_fd, infoString.c_str(), infoString.size(), 0);
    return ip;
}

std::string basicSocketFunctions::RCV_and_PARSE_serverResponse(int socked_fd) // parses OK or ERR responses from server or client
{
    char buffer[1024] = {};
    int err = recv(socked_fd, buffer, sizeof(buffer), 0);
    if (err == -1)
    {
        std::cout << "error in RCV_and_PARSE_serverResponse()" << std::endl;
        return "ERR";
    }
    buffer[err] = '\0';
    std::string buff = buffer;
    if (buff == "OK")
    {
        return "OK";
    }
    else
    {
        if(buff == "TIME"){
            return "TIME";
        }
        return "ERR";
    }
    return "ERR";
}