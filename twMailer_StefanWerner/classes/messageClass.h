#ifndef MESSAGECLASS_H
#define MESSAGECLASS_H

#pragma once

#include "../headers/PresetStruct.h"

class messageClass
{
public:
    messageClass();
    ~messageClass();
    void sendLongMessage(TEXTpreset tp, INFOpreset ip, int socket_fd);
    std::string receiveLongMessage(int client_socket, INFOpreset ip);
private:

};

#endif