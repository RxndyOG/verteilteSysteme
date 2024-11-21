#include "messageClass.h"

#include "../headers/PresetStruct.h"
#include <vector>
#include <sys/socket.h>
#include <iostream>

messageClass::messageClass()
{

}

messageClass::~messageClass()
{

}

std::vector<std::string> splitString(const std::string& input, int textLength) {
    std::vector<std::string> blocks;
    size_t totalLength = textLength;
    
    for (size_t i = 0; i < totalLength; i += (_BLOCK_SIZE-1)) {
        blocks.push_back(input.substr(i, (_BLOCK_SIZE-1)));
    }

    return blocks;
}

void messageClass::sendLongMessage(TEXTpreset tp, INFOpreset ip, int socket_fd) {
    std::string fullMessage = tp.receiver + "\n" + tp.subject + "\n" + tp.message + "\n";
    
    std::vector<std::string> blocks = splitString(fullMessage, ip.textLength);

    do{
        send(socket_fd, blocks[0].c_str(), blocks[0].size(), 0);
        blocks.erase(blocks.begin());
    }while(blocks.size() > 0);
}

std::string messageClass::receiveLongMessage(int client_socket, INFOpreset ip)
{
    std::string fullMessage = "";

    do
    {
        char buffer[_BLOCK_SIZE] = {};
        int err = recv(client_socket, buffer, sizeof(buffer), 0);
        if (err == -1)
        {
            std::cout << "ERROR in packages" << std::endl;
            return "ERR";
        }
        buffer[err] = '\0';
        std::string tmpString = buffer;
        fullMessage.append(tmpString);
        ip.numPack--;
    } while (ip.numPack > 0);
    return fullMessage;
}

