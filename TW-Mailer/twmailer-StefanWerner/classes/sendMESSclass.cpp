#include <string>

#include "sendMESSclass.h"
#include "../headers/PresetsStruct.h"

#include <sys/socket.h>
#include <iostream>

sendMESSclass::sendMESSclass()
{

}

sendMESSclass::~sendMESSclass()
{

}

int sendMESSclass::sendMESSstring(int clientSocket,TextPreset tp)
{
    std::string SENDstring = "";
    SENDstring = SENDstring + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
    send(clientSocket, SENDstring.c_str(), SENDstring.size(), 0);
    return 0;
}

int sendMESSclass::sendMESSstring_Packages(int clientSocket,TextPreset tp)
{
    std::string SENDstring = tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
    int totalLength = SENDstring.size();
    int blockSize = _blockSIZE - 1;
    int sentBytes = 0;

    for (int i = 0; i < tp.packageNUM; i++)
    {
        int remainingBytes = totalLength - sentBytes;
        int currentBlockSize = std::min(blockSize, remainingBytes);
        std::string currentBlock = SENDstring.substr(sentBytes, currentBlockSize);
        currentBlock += '\n';
        int bytesSent = send(clientSocket, currentBlock.c_str(), currentBlock.size(), 0);
        if (bytesSent == -1)
        {
            std::cerr << "Error sending package " << i + 1 << std::endl;
            return -1;
        }
        sentBytes += currentBlockSize;
    }

    return 0;
}