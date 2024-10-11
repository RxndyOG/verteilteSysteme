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
    std::string infoString;
};

enum type
{
    SEND = 1,
    READ = 2,
    LIST = 3,
    DEL = 4,
    QUIT = 5,
    COMMENT = 0,
};

struct TextPreset resetTP(struct TextPreset tp)
{
    tp.sender = "";
    tp.subject = "";
    tp.text = "";
    tp.argument = "";
    tp.packageNUM = 1;
    tp.length = 0;
    tp.type = 0;
    tp.delim = "\n";
    tp.infoString = "";
    tp.error = false;
    tp.ID = 0;
    tp.username = "";
    return tp;
}

struct TextPreset parseINFO(struct TextPreset tp, std::string info)
{
    tp.infoString = info;
    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < info.size(); i++)
    {
        if (info[i] == '\n')
        {
            switch (j)
            {
            case 0:
                tp.type = std::stoi(parseTemp);
                break;
            case 1:
                tp.packageNUM = std::stoi(parseTemp);
                break;
            case 2:
                tp.length = std::stoi(parseTemp);
                break;
            default:

                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += info[i];
        }
    }

    return tp;
}

struct TextPreset parseSEND(struct TextPreset tp, std::string sendMESS)
{
    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < sendMESS.size(); i++)
    {
        if (sendMESS[i] == '\n')
        {
            switch (j)
            {
            case 0:
                tp.argument = parseTemp;
                break;
            case 1:
                tp.sender = parseTemp;
                break;
            case 2:
                tp.subject = parseTemp;
                break;
            default:
                tp.text += parseTemp;
                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += sendMESS[i];
        }
    }

    return tp;
}

void initializeSENDSAVE(struct TextPreset tp, int clientSocket, std::vector<struct TextPreset> &n)
{

    char buffer[1024];
    int errRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRcv] = '\n';

    if (errRcv == -1)
    {
        std::cout << "Error in initialize single pack" << std::endl;
    }

    tp = parseSEND(tp, std::string(buffer));

    n.push_back(tp);
}

void initializeSENDSAVE_Packages(struct TextPreset tp, int clientSocket, std::vector<struct TextPreset> &n)
{
    std::string completeMessage;
    int totalPackages = tp.packageNUM;
    int currentPackage = 1;
    while (currentPackage <= totalPackages)
    {
        char buffer[1024]; 
        int errRcv = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (errRcv == -1)
        {
            std::cerr << "Error in receiving package " << currentPackage << std::endl;
            return;
        }
        completeMessage.append(buffer, errRcv);
        ++currentPackage;
    }

    tp = parseSEND(tp, completeMessage);

    n.push_back(tp);

}

void recvFromClient(int clientSocket, std::vector<struct TextPreset> &n)
{

    struct TextPreset tpRECV;
    tpRECV = resetTP(tpRECV);
    char buffer[1024];
    int errRCV = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRCV] = '\n';
    if (errRCV == -1)
    {
        std::cout << "Error in recvFromClient" << std::endl;
        send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
    }

    tpRECV = parseINFO(tpRECV, std::string(buffer));

    if (tpRECV.type == SEND)
    {
        send(clientSocket, "OK\n", sizeof("OK\n"), 0);

        if (tpRECV.packageNUM == 1)
        {
            initializeSENDSAVE(tpRECV, clientSocket, n);
        }
        else
        {
            initializeSENDSAVE_Packages(tpRECV, clientSocket, n);
        }
    }
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
        recvFromClient(clientSocket, savedMSG);

        // test if saved to vector. it does
        //std::cout << savedMSG[0].text << std::endl;

    } while (!closeFlag);

    close(clientSocket);
    close(serverSocket);

    return 0;
}