
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <vector>
#include <utility>

void sendToClient(int clientSocket, const char *message)
{
    send(clientSocket, message, strlen(message), 0);
}

void SENDmessageRecv(char buffer[4096])
{
    char *pch;
    pch = strtok(buffer, ":");
    while (pch != NULL)
    {
        std::cout << pch << std::endl;
        pch = strtok(NULL, ":");
    }
}

std::pair<std::string, std::vector<std::string>::size_type> READmessagRecv(const std::string &message)
{
    size_t firstColon = message.find(':');
    size_t secondColon = message.find(':', firstColon + 1);

    std::string username = message.substr(firstColon + 1, secondColon - firstColon - 1);
    std::vector<std::string>::size_type messageNumber = std::stoul(message.substr(secondColon + 1));

    return std::make_pair(username, messageNumber);
}

void handleSEND(const std::vector<std::string> &tokens)
{
    // Einfach alle Teile der Nachricht ausgeben
    for (const auto &part : tokens)
    {
        std::cout << part << std::endl;
    }
}

int receiveFromClient(int clientSocket, std::vector<std::string> *savedMsg)
{
    char buffer[4096] = {0};  // Puffer initialisieren und leeren
    int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (errRecv == -1) {
        return -1;
    }

    std::string recvMessage(buffer);  // Puffer in einen std::string umwandeln

    // Überprüfen, ob die Nachricht eine gültige Aktion ist
    if (recvMessage.substr(0, 4) == "QUIT") {
        std::cout << "Client initialisiert quit!\nVerbindung schließen!" << std::endl;
        return -2;
    }

    if (recvMessage.substr(0, 4) == "SEND") {
        std::cout << "Client used SEND" << std::endl;
        savedMsg->push_back(recvMessage);  // Nachricht speichern
        sendToClient(clientSocket, "OK");
        return 1;  // SEND
    }

    if (recvMessage.substr(0, 4) == "READ") {

        std::cout << "Client used READ" << std::endl;
        sendToClient(clientSocket, "OK");
        // Nachricht parsen (wie vorher)
        std::pair<std::string, std::vector<std::string>::size_type> p = READmessagRecv(recvMessage);
        
        if (p.second < savedMsg->size()) {
            std::string messageToSend = (*savedMsg)[p.second];
            sendToClient(clientSocket, messageToSend.c_str());
        } else {
            sendToClient(clientSocket, "ERR: Message not found");
        }
        return 2;  // READ
    }

    std::cout << "Client: " << recvMessage << std::endl;
    return 0;
}


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("error");
    }

    std::vector<std::string> savedMsg;

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Bind the socket
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[1])); // Port number
    serverAddress.sin_addr.s_addr = INADDR_ANY;    // Bind to any available interface

    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return 2;
    }

    // Listen for connections
    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Error listening" << std::endl;
        close(serverSocket);
        return 3;
    }

    std::cout << "Server listening on port " << argv[2] << std::endl;

    // Accept connections
    sockaddr_in clientAddress;
    socklen_t clientSize = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientSize);
    if (clientSocket == -1)
    {
        std::cerr << "Error accepting connection" << std::endl;
        close(serverSocket);
        return 4;
    }

    // Get client's IP address and port
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddress.sin_port);
    std::cout << "Accepted connection from " << clientIP << ":" << clientPort << std::endl;

    // Send data to the client
    sendToClient(clientSocket, "Hello from Server");

    sockaddr_in serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);
    getsockname(clientSocket, (struct sockaddr *)&serverAddr, &serverAddrLen);
    char serverIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverAddr.sin_addr), serverIP, INET_ADDRSTRLEN);

    sendToClient(clientSocket, serverIP);
    receiveFromClient(clientSocket, &savedMsg);

    bool closeFlag = false;

    do
    {
        switch (receiveFromClient(clientSocket, &savedMsg))
        {
        case -2: // QUIT
            closeFlag = true;
            break;
        case -1: // ERROR
            std::cout << "Error happened" << std::endl;
            sendToClient(clientSocket, "ERR");
            break;
        case 0:
            break;
        case 1: // SEND
            //sendToClient(clientSocket, "OK");
            break;
        case 2: // READ

            break;
        default:
            sendToClient(clientSocket, "ERR");
            break;
        }
    } while (!closeFlag);

    close(clientSocket);
    close(serverSocket);

    return 0;
}