#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <utility>
#include <cmath>

enum type
{
    SEND = 1,
    READ = 2,
    LIST = 3,
    DEL = 4,
    QUIT = 5,
    COMMENT = 0,
};

struct INFOStruct{
    int argument;
    std::string username;
    int textLength;
    int packageNUM;
    std::string INFOstring;
};

struct txtStruct{
    std::string subject;
    std::string sender;
    std::string text;
    
};

std::string recvFunctBasic(int clientSocket, int* ERR){
    char buffer[1024] = {0};
    int errRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRcv] = '\0';
    if (errRcv == -1)
    {
        std::cout << "Error in initialize single pack" << std::endl;
        ERR = &errRcv;
    }
    return buffer;
}

void sendFunctBasic(std::string arg, int clientSocket){
    send(clientSocket, arg.c_str(), arg.size(), 0);
}

void calcInfoString(INFOStruct *is, txtStruct *ts){ //infoString formatierung -> argument\nusername\ntextLength\npackageNUM\n
    is->textLength = ts->sender.size() + ts->subject.size() + ts->text.size() + 3;
    is->packageNUM = std::ceil(is->textLength/1024);
    is->INFOstring = std::to_string(is->argument) + "\n" + is->username + "\n" + std::to_string(is->textLength) + "\n" + std::to_string(is->packageNUM) + "\n";
}

void sendInput(txtStruct *ts){

    std::cout << "Sender: ";
    getline(std::cin, ts->sender);
    std::cout << "Subject: ";
    getline(std::cin, ts->subject);
    std::cout << "Message: ";
    std::string tmpString;
    do{
        getline(std::cin, tmpString);
        if(tmpString == "."){break;}
        ts->text.append(tmpString + "\n");
    }while(tmpString == ".");
}

void userInput(INFOStruct *is, txtStruct *ts){
    std::string argument;
    getline(std::cin, argument);
    if(argument.substr(0,4) == "SEND"){
        is->argument = SEND;
        sendInput(ts);
        calcInfoString(is, ts);
    }
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

int SENDoption(INFOStruct *is,txtStruct *ts, int clientSocket){
    std::string completeString = ts->sender + "\n" + ts->subject + "\n" + ts->text + "\n";
    size_t totalLength = completeString.size();

    std::cout << totalLength << " " << is->textLength << std::endl;

    size_t bytesSent = 0;
    const size_t MAX_PACKAGE_SIZE = 1024;

    while (bytesSent < totalLength) {
        size_t bytesToSend = std::min(MAX_PACKAGE_SIZE, totalLength - bytesSent);
        
        std::string block = completeString.substr(bytesSent, bytesToSend);
        
        sendFunctBasic(block, clientSocket);
      
        bytesSent += bytesToSend;

        is->packageNUM--;

        if (is->packageNUM <= 0) break;
    }
    return 0;
}

int calcDirection(INFOStruct *is,txtStruct *ts, int clientSocket){
    switch(is->argument){
        case SEND:
            SENDoption(is, ts, clientSocket);
            delete ts;
            break;
        case READ:

            delete ts;
            break;
        case LIST:

            delete ts;
            break;
        case QUIT:

            delete ts;
            return QUIT;
            break;
    }
    return COMMENT;
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
    buffer[errRecv] = '\0';
    if (errRecv == -1)
    {
        std::cout << "error happened during recv from server" << std::endl;
    }
    // receiveFromServer(clientSocket,false);
    // sendToServer(clientSocket, "Hello from the Client", &sendMSGS);

    int startINPUT;

    do
    {
        INFOStruct *is = new INFOStruct();
        txtStruct *ts = new txtStruct();
        std::cout << "\n --------- Open Terminal ----------" << std::endl;
        std::cout << "[SEND] [READ] [LIST] [DEL] [QUIT]" << std::endl;
        userInput(is,  ts);         //ask users for SEND, READ, LIST etc...
        sendFunctBasic(is->INFOstring,clientSocket);    // send info string for option above
        int *ERR = 0;
        std::string buffer = recvFunctBasic(clientSocket, ERR); // recv OK\n from 
        std::cout << buffer << std::endl;
        if(buffer != "OK\n"){std::cout << "No OK from server" << std::endl; return -1;}
        startINPUT = calcDirection(is, ts, clientSocket);

        delete ts;
        delete is;
    } while (startINPUT != QUIT);

    std::cout << "quiting in prozess" << std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}