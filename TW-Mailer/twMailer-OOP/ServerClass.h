#ifndef SERVERCLASS_H
#define SERVERCLASS_H

#pragma once

class ServerClass
{
public:
    ServerClass();
    ~ServerClass();

    int CreateSocket();
    int BindSocket(int i);
    int ListenSockets(char* arg);
    int AcceptSocket();

    int UserIP();

    int RecvFromClient();

    int GetServerSocket();
    int GetClientSocket();

private:

    std::string filePath;
    int serverSocket;
    int clientSocket;
    sockaddr_in serverAddress = {};
    sockaddr_in clientAddress = {};
    socklen_t clientSize;
};

#endif