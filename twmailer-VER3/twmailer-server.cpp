#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <vector>
#include <utility>

struct TextPreset
{
    int packageNUM;
    std::string delim;
    int length;
    int type;
    std::string argument;
    std::string sender;
    std::string subject;
    std::string text;
};

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

struct TextPreset handleSEND(struct TextPreset tp, int clientSocket)
{

    char buffer[4096] = {0};
    int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\n';

    if (errRecv == -1)
    {
        std::cout << "Error in parse" << std::endl;
        return tp;
    }

    std::string parseMessage = buffer;
    std::vector<std::string> parseMessVect;
    std::string tempMess = "";

    for (long unsigned int i = 0; i < parseMessage.size(); i++)
    {
        if (parseMessage[i] == '\n')
        {
            parseMessVect.push_back(tempMess);
            tempMess.clear();
        }
        else
        {
            tempMess += parseMessage[i];
        }
    }

    for (long unsigned int i = 0; i < parseMessVect.size(); i++)
    {
        if (i == 0)
        {
            tp.argument = parseMessVect[i];
        }
        if (i == 1)
        {
            tp.sender = parseMessVect[i];
        }
        if (i == 2)
        {
            tp.subject = parseMessVect[i];
        }
        if(i > 2){
            tp.text += parseMessVect[i];
        }
    }

    int j = tp.packageNUM;

    if (tp.packageNUM > 1)
    {

        do
        {
            char tempBuffer[4096] = {0};
            int tempErrRecv = recv(clientSocket, tempBuffer, sizeof(tempBuffer), 0);
            tempBuffer[tempErrRecv] = '\n';

            std::string tempParseMessage(buffer);
            std::vector<std::string> tempParseMessVect;
            std::string tempTempMess = "";

            for (long unsigned int i = 0; i < tempParseMessage.size(); i++)
            {
                if (tempParseMessage[i] == '\n')
                {
                    tempParseMessVect.push_back(tempTempMess);
                    tempTempMess.clear();
                }
                else
                {
                    tempTempMess += tempParseMessage[i];
                }
            }

            for (long unsigned int i = 0; i < tempParseMessVect.size(); i++)
            {
                tp.text += tempParseMessVect[i];
            }

            j--;

        } while (j > 1);
    }

    std::cout << "argument: " << tp.argument << " sender: " << tp.sender << " subject: " << tp.subject << std::endl;
    std::cout << "text: " << tp.text << std::endl;

    return tp;
}

int recvFromClient(int clientSocket)
{
    char buffer[4096] = {0}; // Puffer initialisieren und leeren
    int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\0';
    if (errRecv == -1)
    {
        std::cout << "Recv error" << std::endl;
        return 0;
    }

    struct TextPreset tp;

    std::string messageRecv = buffer;
    std::vector<std::string> parseMess;
    std::string tempMESS;
    for (long unsigned int i = 0; i < messageRecv.size(); i++)
    {
        if (messageRecv[i] == '\n')
        {
            parseMess.push_back(tempMESS);
            tempMESS.clear();
        }
        else
        {
            tempMESS += messageRecv[i];
        }
    }
    char trueDelim = '\n';
    if (parseMess.size() >= 4)
    {
        tp.packageNUM = std::stoi(parseMess[0]);
        tp.delim = parseMess[1];
        tp.length = std::stoi(parseMess[2]);
        tp.type = std::stoi(parseMess[3]);
        if (tp.delim == "\\n")
        {
            trueDelim = '\n';
            tp.delim = trueDelim;
        }
    }
    else
    {
        std::cerr << "Error in vector creation" << std::endl;
    }

    switch (tp.type)
    {
    case 0:
        std::cout << "Client sent a Comment";
        return 0;
        break;
    case 1:
        std::cout << "Client used SEND" << std::endl;
        std::cout << "packages: " << tp.packageNUM << " length: " << tp.length << " type: " << tp.type << " delim: " << tp.delim << std::endl;
        send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        handleSEND(tp, clientSocket);
        return 1;
        break;
    case 2:
        std::cout << "Client used READ" << std::endl;
        return 2;
        break;
    default:
        std::cout << "Error happened" << std::endl;
        send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
        return -1;
        break;
    }

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

    // sendToClient(clientSocket, serverIP);
    // receiveFromClient(clientSocket, &savedMsg);

    bool closeFlag = false;

    do
    {
        recvFromClient(clientSocket);
    } while (!closeFlag);

    close(clientSocket);
    close(serverSocket);

    return 0;
}