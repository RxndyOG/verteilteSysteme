#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <utility>
#include <cmath>

int _blockSIZE = 1024;

// das Struct das überall benutzt wird um Messages zu parsen zu schicken und zu speichern
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

//Enum der dem Server bzw User sagt welchen Typ die nachricht hat
enum type
{
    SEND = 1,
    READ = 2,
    LIST = 3,
    DEL = 4,
    QUIT = 5,
    COMMENT = 0,
};

char* recvBufferFromClient(int clientSocket){
    static char buffer[1024] = {0};
    int errRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRcv] = '\n';
    if(errRcv == -1){
        std::cout << "Error - RECV from client" << std::endl;
    }
    return buffer;
}

int calculatePackageNum(size_t messageSize) {
    return (messageSize <= static_cast<long unsigned int>(_blockSIZE)) ? 1 : std::ceil(static_cast<double>(messageSize) / static_cast<long unsigned int>(_blockSIZE));
}

// berechnet den Info string
struct TextPreset calcINFOstring(struct TextPreset tp, int type)
{
    tp.type = type;
    std::string tempString = "";
    tp.infoString.clear();

    switch (tp.type)
    {
    case SEND:
        tempString = tempString + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
        tp.length = tempString.size();
        tp.packageNUM = calculatePackageNum(tp.length);
        tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(tp.packageNUM) + "\n" + std::to_string(tp.length) + "\n";
        break;
    case READ:
        tempString = tempString + tp.username + "\n" + std::to_string(tp.ID) + "\n";
        tp.length = tempString.size();
        tp.packageNUM = calculatePackageNum(tp.length);
        tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(tp.packageNUM) + "\n" + std::to_string(tp.length) + "\n";
        break;
    default:
        break;
    }

    return tp;
}

// resetet den Struct um ihn wieder zu benutzen. Ist nicht wirklich nötig, wird bei der endabgabe nicht vorhanden sein.
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

// parsed den INFO string
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

// parsed den SEND string
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

// parsed die READ message des Users
struct TextPreset parseREAD(struct TextPreset tp, std::string sendMESS)
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
                tp.username = parseTemp;
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
            parseTemp += sendMESS[i];
        }
    }
    return tp;
}

// sendet die Message für den User bei 1 Package
int sendMESSstring(int clientSocket, struct TextPreset tp)
{

    std::string SENDstring = "";
    SENDstring = SENDstring + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
    send(clientSocket, SENDstring.c_str(), SENDstring.size(), 0);

    return 0;
}

// sendet die Message für den User bei mehreren Packages
int sendMESSstring_Packages(int clientSocket, struct TextPreset tp)
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

// sendet den Info string
int sendINFOstring(int clientSocket, struct TextPreset tp)
{
    send(clientSocket, tp.infoString.c_str(), tp.infoString.size(), 0);
    char buffer[1024] = {0};
    strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));
    if (std::string(buffer) == "ERR\n")
    {
        std::cout << "Error in server on send of info string" << std::endl;
    }
    else
    {
        std::cout << buffer << std::endl;
    }
    return 0;
}

// speichert den vom user erhaltenen Message string bei 1 package
void initializeSENDSAVE(struct TextPreset tp, int clientSocket, std::vector<struct TextPreset> &n)
{
    char buffer[1024] = {0};
    strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));
    tp = parseSEND(tp, std::string(buffer));
    n.push_back(tp);
}

// speichert den vom user erhaltenen Message string bei mehreren packages
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

// wird benutzt um die READ message des Users zu verarbeiten und sendet zurück 
void initializeREAD(struct TextPreset tp, int clientSocket, std::vector<struct TextPreset> &n)
{
    char buffer[1024] = {0};
    strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));

    tp = parseREAD(tp, std::string(buffer));
    if (static_cast<long unsigned int>(tp.ID) > n.size())
    {
        std::cout << "Error in initializeREAD ID not existing" << std::endl;
        return;
    }
    else
    {
        if (tp.username == n[tp.ID].sender)
        {
            tp.argument = "READ";
            tp.sender = n[tp.ID].sender;
            tp.subject = n[tp.ID].subject;
            tp.text = n[tp.ID].text;
        }
    }

    std::string READstring = "";
    READstring = READstring + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";

    tp = calcINFOstring(tp, SEND);
    sendINFOstring(clientSocket, tp);

    if (tp.packageNUM == 1)
    {
        sendMESSstring(clientSocket, tp);
    }
    else
    {
        sendMESSstring_Packages(clientSocket, tp);
    }
}

// wird benutzt um die DEL option des users zu verarbeiten
void initializeDEL(struct TextPreset tp, int clientSocket, std::vector<struct TextPreset> &n)
{
    char buffer[1024] = {0};
    strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));

    tp = parseREAD(tp, std::string(buffer));

    if (static_cast<long unsigned int>(tp.ID) <= n.size())
    {
        if (tp.username == n[tp.ID].sender)
        {
            n.erase(n.begin() + tp.ID);
            std::cout << "email deleted" << std::endl;
            send(clientSocket, "OK\n", sizeof("OK\n"), 0);
            return;
        }
        else
        {
            std::cout << "email couldnt be deleted. SENDER not correct" << std::endl;
            send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
            return;
        }
    }
    else
    {
        std::cout << "email couldnt be deleted. ID not correct" << std::endl;
        send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
        return;
    }
}

// sended den LIST string für den User
void LISTsendFunct(int clientSocket, std::vector<struct TextPreset> &n, struct TextPreset tp)
{
    int totalAmmountMESS = n.size();

    std::string LISTstring = "";

    bool sendUSERLIST = false;

    for (int i = 0; i < totalAmmountMESS; i++)
    {
        if (n[i].sender == tp.sender)
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
    strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));

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
int recvFromClient(int clientSocket, std::vector<struct TextPreset> &n)
{

    struct TextPreset tpRECV;
    tpRECV = resetTP(tpRECV);
    char buffer[1024] = {0};
    strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));

    tpRECV = parseINFO(tpRECV, std::string(buffer));

    switch (tpRECV.type)
    {
    case SEND:
        send(clientSocket, "OK\n", sizeof("OK\n"), 0);

        if (tpRECV.packageNUM == 1)
        {
            initializeSENDSAVE(tpRECV, clientSocket, n);
        }
        else
        {
            initializeSENDSAVE_Packages(tpRECV, clientSocket, n);
        }
        return SEND;
        break;
    case READ:
        if (n.size() <= 0)
        {
            send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
        }
        else
        {
            send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        }
        initializeREAD(tpRECV, clientSocket, n);
        return READ;
        break;
    case LIST:
        if (n.size() == 0)
        {
            send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
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
        if (n.size() <= 0)
        {
            send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
        }
        else
        {
            send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        }
        initializeDEL(tpRECV, clientSocket, n);
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

    int cancell;
    do
    {
        cancell = recvFromClient(clientSocket, savedMSG);

    } while (cancell != QUIT);

    std::cout << "client closed server" << std::endl;

    close(clientSocket);
    close(serverSocket);

    return 0;
}