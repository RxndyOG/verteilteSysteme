#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>

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
    std::string infoString;
    std::string username;
    int ID;
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

// resetet den Struct um ihn wieder zu benutzen. Ist nicht wirklich nötig, wird bei der endabgabe nicht vorhanden sein.
struct TextPreset resetTP(struct TextPreset tp)
{
    tp.sender = "";
    tp.subject = "";
    tp.text = "";
    tp.argument = "";
    tp.packageNUM = 0;
    tp.length = 0;
    tp.type = 0;
    tp.delim = "\n";
    tp.infoString = "";
    tp.username = "";
    tp.ID = 0;
    return tp;
}

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

struct TextPreset setInfoString(struct TextPreset tp, std::string tempString){
        tp.length = tempString.size();
        tp.packageNUM = calculatePackageNum(tp.length);
        tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(tp.packageNUM) + "\n" + std::to_string(tp.length) + "\n";
    return tp;
}

// berechnet den String der zuerst zum Server bzw Client geschickt werden soll damit sie wissen wie lange die nächste gesendete Message ist
struct TextPreset calcINFOstring(struct TextPreset tp, int type)
{
    tp.type = type;
    std::string tempString = "";

    switch (tp.type)
    {
    case SEND:
        tempString = tempString + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
        tp = setInfoString(tp, tempString);
        break;
    case READ:
        tempString = tempString + tp.username + "\n" + std::to_string(tp.ID) + "\n";
        tp = setInfoString(tp, tempString);
        break;
    case QUIT:
        tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(1) + "\n" + std::to_string(1) + "\n";
        break;
    case LIST:
        tempString = tempString + tp.username + "\n" + std::to_string(tp.ID) + "\n";
        tp = setInfoString(tp, tempString);
        break;
    default:
        tempString = tempString + tp.username + "\n" + std::to_string(tp.ID) + "\n";
        tp = setInfoString(tp, tempString);
        break;
    }

    return tp;
}

// wird benutzt um die vom user gewollte message zu bekommen
struct TextPreset SENDInput(struct TextPreset tp)
{

    std::cout << "sender: ";
    std::getline(std::cin, tp.sender);
    std::cout << "subject: ";
    std::getline(std::cin, tp.subject);
    std::string currentMESS = "";
    std::cout << "message: ";
    do
    {
        currentMESS.clear();
        std::getline(std::cin, currentMESS);
        tp.text += (currentMESS + '\n');
    } while (currentMESS != ".");

    return tp;
}

// Sendet den Info string
int sendINFOstring(int clientSocket, struct TextPreset tp)
{
    send(clientSocket, tp.infoString.c_str(), tp.infoString.size(), 0);
    char buffer[1024] = {0};
    strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));
    if (std::string(buffer) == "ERR\n")
    { std::cout << "Error in sendINFOstring: not enough elements" << std::endl; return -1; }
    else
    { std::cout << buffer << std::endl; }
    return 0;
}

// sendet die eigene Message wenn sie ein package hat
int sendMESSstring(int clientSocket, struct TextPreset tp)
{
    std::string SENDstring = "";
    SENDstring = SENDstring + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
    send(clientSocket, SENDstring.c_str(), SENDstring.size(), 0);
    return 0;
}

// sendet die eigene Message wenn sie mehrere Packages hat
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

// parsed die LIST funktion
void parseLIST(std::string text)
{

    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < text.size(); i++)
    {
        if (text[i] == '\n')
        {
            switch (j % 4)
            {
            case 0:
                std::cout << "ID: " << parseTemp << std::endl;
                break;
            case 1:
                std::cout << "SENDER: " << parseTemp << std::endl;
                break;
            case 2:
                std::cout << "SUBJECT: " << parseTemp << std::endl;
                break;
            case 3:
                std::cout << "TEXT (first 10): " << parseTemp << std::endl;
                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += text[i];
        }
    }
}

// bekommt die LIST option vom server und printet sie
void recvLISTprint(int clientSocket)
{
    char buffer[1024] = {0};
    strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));

    if (std::string(buffer) == "ERR\n")
    {
        std::cout << "NO email with given username exists" << std::endl;
        return;
    }

    parseLIST(buffer);

    return;
}

// parsed den Info String
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

// parsed den READ string bzw option
struct TextPreset parseREAD(struct TextPreset tp, std::string text)
{

    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < text.size(); i++)
    {
        if (text[i] == '\n')
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
                tp.text += '\n';
                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += text[i];
        }
    }

    return tp;
}

// speichert den vom SEND bekommenen string als struct ab
void initializeSENDSAVE(struct TextPreset tp, int clientSocket)
{
    char buffer[1024] = {0};
    strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));
    tp = parseREAD(tp, std::string(buffer));

    std::cout << tp.argument << std::endl;
    std::cout << tp.sender << std::endl;
    std::cout << tp.subject << std::endl;
    std::cout << tp.text << std::endl;
}

// speichert den vom SEND bekommenen string ab wenn er mehr als 1 package hat
void initializeSENDSAVE_Packages(struct TextPreset tp, int clientSocket)
{
    std::string completeMessage;
    int totalPackages = tp.packageNUM;

    int currentPackage = 1;
    while (currentPackage <= totalPackages)
    {

        char buffer[1024] = {0};
        int errRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[errRcv] = '\n';

        if (errRcv == -1)
        {
            std::cerr << "Error in receiving package " << currentPackage << std::endl;
            return;
        }
        completeMessage.append(buffer, errRcv);
        ++currentPackage;
    }

    tp = parseREAD(tp, completeMessage);

    std::cout << tp.argument << std::endl;
    std::cout << tp.sender << std::endl;
    std::cout << tp.subject << std::endl;
    std::cout << tp.text << std::endl;
}

// fragt den user nach dem Input wenn er READ angegeben hat
struct TextPreset READinput(struct TextPreset tp)
{
    std::cout << "Username: ";
    std::getline(std::cin, tp.username);
    std::cout << "ID: ";
    std::string tempID;
    std::getline(std::cin, tempID);
    tp.ID = std::stoi(tempID);
    return tp;
}

// diese funktion ist eigentlich dafür da das der User erkannt wird beim READ mittels der ID
void sendIPREAD(struct TextPreset tp, int clientSocket)
{
    std::string READstring = "";
    READstring = READstring + tp.username + "\n" + std::to_string(tp.ID) + "\n";
    send(clientSocket, READstring.c_str(), READstring.size(), 0);
}

// sendet die LIST option zum server
struct TextPreset sendLISTuser(int clientSocket, struct TextPreset tp)
{
    std::string LISTstring = "";
    LISTstring = LISTstring + tp.username + "\n" + std::to_string(tp.ID) + "\n";
    send(clientSocket, LISTstring.c_str(), LISTstring.size(), 0);
    return tp;
}

// fragt den USer nach dem Input wenn er LIST ausgewählt hat
struct TextPreset LISTinput(struct TextPreset tp)
{
    std::cout << "username: ";
    std::getline(std::cin, tp.username);
    return tp;
}

// verarbeitet den Input vom user Also was er gewählt hat ob SEND, LIST, READ etc...
int userINPUTfindOpt(int clientSocket)
{
    std::string input = "";
    struct TextPreset tpInput = resetTP({});
    std::getline(std::cin, input);
    if (input.substr(0, 4) == "SEND")
    {
        tpInput = SENDInput(tpInput);
        tpInput = calcINFOstring(tpInput, SEND);
        sendINFOstring(clientSocket, tpInput);
        if (tpInput.packageNUM == 1)
        {
            sendMESSstring(clientSocket, tpInput);
        }
        else
        {
            sendMESSstring_Packages(clientSocket, tpInput);
        }
        return SEND;
    }
    else if (input.substr(0, 4) == "READ")
    {
        tpInput = READinput(tpInput);
        tpInput = calcINFOstring(tpInput, READ);
        int testLIST = sendINFOstring(clientSocket, tpInput);
        if (testLIST == -1)
        {
            std::cout << "Not enough Elements to LIST" << std::endl;
            return READ;
        }
        sendIPREAD(tpInput, clientSocket);

        char buffer[1024] = {0};
        strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));

        tpInput = parseINFO(tpInput, std::string(buffer));
        send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        if (tpInput.packageNUM == 1)
        {
            initializeSENDSAVE(tpInput, clientSocket);
        }
        else
        {
            initializeSENDSAVE_Packages(tpInput, clientSocket);
        }
        return READ;
    }
    else if (input.substr(0, 4) == "LIST")
    {
        tpInput = LISTinput(tpInput);
        tpInput = calcINFOstring(tpInput, LIST);
        int testLIST = sendINFOstring(clientSocket, tpInput);
        if (testLIST == -1)
        {
            std::cout << "Not enough Elements to LIST" << std::endl;
            return LIST;
        }
        tpInput = sendLISTuser(clientSocket, tpInput);
        recvLISTprint(clientSocket);
        return LIST;
    }
    else if (input.substr(0, 3) == "DEL")
    {
        tpInput = READinput(tpInput);
        tpInput = calcINFOstring(tpInput, DEL);
        int testLIST = sendINFOstring(clientSocket, tpInput);
        if (testLIST == -1)
        {
            std::cout << "Not enough Elements to LIST" << std::endl;
            return DEL;
        }
        sendIPREAD(tpInput, clientSocket);

        char buffer[1024] = {0};
        strncpy(buffer, recvBufferFromClient(clientSocket), sizeof(buffer));

        if(std::string(buffer) == "ERR\n"){
            std::cout << "item couldnt be deleted" << std::endl;
        }else if(std::string(buffer) == "OK\n"){
            std::cout << "item deleted" << std::endl;
        }

    }
    else if (input.substr(0, 4) == "QUIT")
    {
        tpInput = calcINFOstring(tpInput, QUIT);
        int testLIST = sendINFOstring(clientSocket, tpInput);
        if (testLIST == -1)
        {
            return QUIT;
        }
        return QUIT;
    }
    else
    {
        send(clientSocket, input.c_str(), sizeof(input.c_str()), 0);
    }

    return 0;
}

// wird für die IP benutzt nicht so wichtig schaut nur ob der user localhost eingegeben hat
std::string convertToLowercase(const std::string &str)
{
    std::string result = "";
    for (char ch : str)
    {
        result += tolower(ch);
    }
    return result;
}

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        printf("error");
    }

    // int sendMSGS = 0;
    std::string ipAddress = argv[1];
    if (convertToLowercase(ipAddress) == "localhost")
    {
        ipAddress = "127.0.0.1";
    }

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Connect to the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2]));                // Port number
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str()); // Server IP

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Error connecting to server" << std::endl;
        close(clientSocket);
        return 2;
    }

    std::cout << "listening on Port: " << argv[2] << std::endl;

    // Receive data from the serve
    char buffer[1024] = {0};
    int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\n';
    if (errRecv == -1)
    {
        std::cout << "error happened during recv from server" << std::endl;
    }
    // receiveFromServer(clientSocket,false);
    // sendToServer(clientSocket, "Hello from the Client", &sendMSGS);

    int startINPUT;

    do
    {
        std::cout << "\n --------- Open Terminal ----------" << std::endl;
        std::cout << "[SEND] [READ] [LIST] [DEL] [QUIT]" << std::endl;

        startINPUT = userINPUTfindOpt(clientSocket);

    } while (startINPUT != QUIT);

    std::cout << "quiting in prozess" << std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}