#ifndef CLIENTCLASS_H
#define CLIENTCLASS_H

#pragma once

#include <netinet/in.h>  // Add this header


class ClientClass
{
public:
    ClientClass();
    ~ClientClass();
    int CreateSocket();
    int ConnectSocket(int port,const char* adress);

    int GetClientSocket();

private:
    int clientSocket;
    sockaddr_in serverAddress;
};

#endif