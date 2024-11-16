#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <utility>
#include <cmath>

#include "headers/PresetsStruct.h"
#include "classes/parseClass.h"
#include "classes/initializeClass.h"
#include "classes/sendMESSclass.h"
#include "classes/basicFunktions.h"
#include "classes/FileHandeling.h"

// sended den LIST string für den User
void LISTsendFunct(int clientSocket, std::vector<struct TextPreset> &n, struct TextPreset tp)
{
    int totalAmmountMESS = n.size();

    std::string LISTstring = "";

    bool sendUSERLIST = false;

    for (int i = 0; i < totalAmmountMESS; i++)
    {
        if (n[i].username == tp.sender)
        {
            LISTstring = LISTstring + std::to_string(i) + "\n" + n[i].sender + "\n" + n[i].subject + "\n" + n[i].text.substr(0, 10) + "\n";
            sendUSERLIST = true;
        }
    }

    if (LISTstring.size() > static_cast<long unsigned int>(_blockSIZE - 1))
    {
        std::cout << "LIST zu groSS" << std::endl;
    }

    if (sendUSERLIST == false)
    {
        send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
    }
    else
    {
        send(clientSocket, LISTstring.c_str(), LISTstring.size(), 0);
    }
}

// bekommt die LIST anfrage des users und verarbeitet sie
struct TextPreset recvLISTstring(struct TextPreset tp, int clientSocket)
{
    char buffer[1024] = {0};
    int errRCV = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRCV] = '\0';
    if (errRCV == -1)
    {
        std::cout << "Error in recvLISTstring" << std::endl;
        send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
    }

    std::string LISTstring = buffer;
    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < LISTstring.size(); i++)
    {
        if (LISTstring[i] == '\n')
        {
            switch (j)
            {
            case 0:
                tp.sender = parseTemp;
                break;
            case 1:
                tp.ID = std::stoi(parseTemp);
                break;
            default:
                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += LISTstring[i];
        }
    }

    return tp;
}

// wird benutzt um zu testen was der User als option ausgewählt hat. Also SEND, READ, etc...
int recvFromClient(int clientSocket, std::vector<struct TextPreset> &n, std::string local)
{

    struct TextPreset tpRECV;
    tpRECV = resetTP(tpRECV);
    tpRECV.fileLocal = local;
    char buffer[1024] = {0};
    int errRCV = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRCV] = '\0';
    if (errRCV == -1)
    {
        std::cout << "Error in recvFromClient" << std::endl;
        send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
    }
    tpRECV = parseClass().parseINFO(tpRECV, std::string(buffer));

    switch (tpRECV.type)
    {
    case SEND:
        send(clientSocket, "OK\n", sizeof("OK\n"), 0);

        if (tpRECV.packageNUM == 1)
        {
            initializeClass().initializeSENDSAVE(tpRECV, clientSocket, n);
        }
        else
        {
            initializeClass().initializeSENDSAVE_Packages(tpRECV, clientSocket, n);
        }
        return SEND;
        break;
    case READ:
        if (n.size() <= 0){send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);}else{send(clientSocket, "OK\n", sizeof("OK\n"), 0);}
        initializeClass().initializeREAD(tpRECV, clientSocket, n);
        return READ;
        break;
    case LIST:
        if (n.size() == 0)
        {
            send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
            return LIST;
        }
        else
        {
            send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        }
        tpRECV = recvLISTstring(tpRECV, clientSocket);
        LISTsendFunct(clientSocket, n, tpRECV);
        return LIST;
        break;
    case DEL:
        if (n.size() <= 0){
            send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
        }
        else{
            send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        }
        initializeClass().initializeDEL(tpRECV, clientSocket, n);
        return DEL;
        break;
    case QUIT:
        send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        return QUIT;
        break;
    default:
        std::cout << "CLIENT: " << buffer << std::endl;
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

    std::vector<struct TextPreset> savedMSG;

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
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);    // Bind to any available interface

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

    std::cout << "Server listening on port " << argv[1] << std::endl;

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
    
    std::string username = basicFunktions().recvFunctBasic(clientSocket);

    if(username == "EMPTYSTRINGUSERNAME"){
        username = "";
        std::cout << "anonimous user" << std::endl;
    }else{
        std::string arg = FileHandeling().readFileToString(argv[2], username);
        savedMSG = FileHandeling().parseFile(arg);

    }

    
    int cancell;
    do
    {
        cancell = recvFromClient(clientSocket, savedMSG, argv[2]);
    } while (cancell != QUIT);

    std::cout << "client closed server" << std::endl;

    close(clientSocket);
    close(serverSocket);

    return 0;
}