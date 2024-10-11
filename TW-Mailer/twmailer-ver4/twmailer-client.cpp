#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>

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
};

int _blockSIZE = 1024;

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
    tp.packageNUM = 0;
    tp.length = 0;
    tp.type = 0;
    tp.delim = "\n";
    tp.infoString = "";
    return tp;
}

struct TextPreset calcINFOstring(struct TextPreset tp, int type)
{

    std::string tempString = "";

    tempString = tempString + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
    tp.length = tempString.size();
    tp.type = type;

    if (tempString.size() <= static_cast<long unsigned int>(_blockSIZE))
    {
        tp.packageNUM = 1;
    }
    else
    {
        double rounder = static_cast<double>(tempString.size()) / static_cast<double>(_blockSIZE);
        tp.packageNUM = std::ceil(rounder);
    }

    tp.infoString = tp.infoString + std::to_string(tp.type) + "\n" + std::to_string(tp.packageNUM) + "\n" + std::to_string(tp.length) + "\n";
    return tp;
}

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
        tp.text += currentMESS;
    } while (currentMESS != ".");

    return tp;
}

int sendINFOstring(int clientSocket, struct TextPreset tp)
{

    send(clientSocket, tp.infoString.c_str(), tp.infoString.size(), 0);
    char buffer[1024];
    int errRCV = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRCV] = '\n';
    if (errRCV == -1)
    {
        std::cout << "Error in senINFOstring" << std::endl;
    }
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

int sendMESSstring(int clientSocket, struct TextPreset tp)
{

    std::string SENDstring = "";
    SENDstring = SENDstring + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
    send(clientSocket, SENDstring.c_str(), SENDstring.size(), 0);

    return 0;
}

int sendMESSstring_Packages(int clientSocket, struct TextPreset tp)
{
    std::string SENDstring = tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";
    int totalLength = SENDstring.size();
    int blockSize = _blockSIZE -1; 
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

int userINPUTfindOpt(int clientSocket)
{
    std::string input;
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
    }
    else if (input.substr(0, 4) == "READ")
    {
    }
    else if (input.substr(0, 4) == "LIST")
    {
        tpInput = calcINFOstring(tpInput, LIST);
    }
    else if (input.substr(0, 3) == "DEL")
    {
    }
    else if (input.substr(0, 4) == "QUIT")
    {
        return -1;
    }
    else
    {
        send(clientSocket, input.c_str(), sizeof(input.c_str()), 0);
    }

    return 0;
}

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
        std::cout << "[SEND] [READ] [DEL] [QUIT]" << std::endl;

        startINPUT = userINPUTfindOpt(clientSocket);

    } while (startINPUT != -1);

    std::cout << "quiting in prozess" << std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}