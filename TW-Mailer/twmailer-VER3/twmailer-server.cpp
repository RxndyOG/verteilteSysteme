#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <vector>
#include <utility>
#include <cmath>
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
    bool error;
    int ID;
    std::string username;
};

struct TextPreset handleSEND(struct TextPreset tp, int clientSocket)
{
    char buffer[1024] = {0};
    int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\n';
    tp.error = false;

    if (errRecv == -1)
    {
        std::cout << "Error in parse" << std::endl;
        tp.error = true;
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
        if (i > 2)
        {
            tp.text += parseMessVect[i];
        }
    }

    int j = tp.packageNUM;

    if (tp.packageNUM > 1)
    {

        do
        {
            char tempBuffer[1024] = {0};
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

        } while (j >= 1);
    }
    else
    {
        char tempBuffer[1024] = {0};
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
    }

    tp.error = false;
    return tp;
}

struct TextPreset handleLIST(std::vector<struct TextPreset> &savedMsg, int clientSocket)
{

    struct TextPreset tp;

    char buffer[1024] = {0};
    int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\n';
    tp.error = false;

    if (errRecv == -1)
    {
        std::cout << "Error in parse" << std::endl;
        send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
        tp.error = true;
        return tp;
    }

    std::string LISTstring = "";

    for (struct TextPreset &i : savedMsg)
    {
        LISTstring = LISTstring + std::to_string(i.ID) + ": " + std::to_string(i.packageNUM) + ": \n   " + i.sender + ": " + i.subject + ": " + i.text.substr(0, 10) + "... \n   " + std::to_string(i.length) + "\n";
    }

    LISTstring += "END\n";

    double roundErr = 0;

    tp.length = LISTstring.size();
    tp.type = 3;
    tp.delim = '\n';
    tp.packageNUM = 1;

    if (LISTstring.size() > 1023)
    {
        roundErr = LISTstring.size() / 1024;
        tp.packageNUM = std::ceil(roundErr);
        std::string infoString = "";
        infoString = std::to_string(tp.packageNUM) + '\n' + tp.delim + '\n' + std::to_string(tp.length) + '\n' + std::to_string(tp.type) + '\n';
        send(clientSocket, infoString.c_str(), infoString.size(), 0);

        std::string clippedString = "";

        int i = 1;
        do
        {

            int startIndex = (i - 1) * 1023;
            int endIndex = std::min(startIndex + 1023, tp.length);

            clippedString = LISTstring.substr(startIndex, endIndex - startIndex);

            send(clientSocket, clippedString.c_str(), clippedString.size(), 0);

            i++;
        } while (i <= tp.packageNUM + 1);
    }
    else
    {

        std::string infoString = "";
        infoString = std::to_string(tp.packageNUM) + '\n' + tp.delim + '\n' + std::to_string(tp.length) + '\n' + std::to_string(tp.type) + '\n';
        send(clientSocket, infoString.c_str(), infoString.size(), 0);
        send(clientSocket, LISTstring.c_str(), LISTstring.size(), 0);
    }
    return tp;
}

struct TextPreset parseInfoString(struct TextPreset tp, char buffer[1024])
{

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

    return tp;
}

int recvFromClient(int clientSocket, std::vector<struct TextPreset> &savedMsg)
{
    char buffer[1024] = {0}; // Puffer initialisieren und leeren
    int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\0';
    if (errRecv == -1)
    {
        std::cout << "Recv error" << std::endl;
        return 0;
    }

    struct TextPreset tp;

    tp = parseInfoString(tp, buffer);

    switch (tp.type)
    {
    case 0:
        std::cout << "Client sent a Comment";
        return 0;
        break;
    case 1:
        std::cout << "Client used SEND" << std::endl;
        send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        savedMsg.push_back(handleSEND(tp, clientSocket));
        /*
        if(savedMsg[savedMsg.size()].error == true){
            std::cout << "Error during parse of Message" << std::endl;
            std::cout << "Deleting broken Message to preserve Securitry" << std::endl;
            send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
            savedMsg.erase(savedMsg.begin() + savedMsg.size());
        }else{
            send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        }
        */
        return 1;
        break;
    case 2:
        std::cout << "Client used READ" << std::endl;
        return 2;
        break;
    case 3:
        std::cout << "Client used LIST" << std::endl;
        send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        handleLIST(savedMsg, clientSocket);
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

    std::vector<struct TextPreset> savedMsg;

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
    send(clientSocket, "Hello from Server", sizeof("Hello from Server"), 0);

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
        recvFromClient(clientSocket, savedMsg);
    } while (!closeFlag);

    close(clientSocket);
    close(serverSocket);

    return 0;
}