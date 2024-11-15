#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>

#include "headers/PresetsStruct.h"
#include "classes/parseClass.h"
#include "classes/initializeClass.h"
#include "classes/sendMESSclass.h"
#include "classes/basicFunktions.h"

// wird benutzt um die vom user gewollte message zu bekommen
struct TextPreset SENDInput(struct TextPreset tp)
{

    std::cout << "sender: ";
    if(tp.username == ""){
        std::getline(std::cin, tp.sender);
        tp.username = tp.sender;
    }else{
        tp.sender = tp.username;
        std::cout << tp.username << std::endl;
    }
    
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

// bekommt die LIST option vom server und printet sie
void recvLISTprint(int clientSocket)
{
    char buffer[1024] = {0};
    int errRCV = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRCV] = '\0';
    if (errRCV == -1){
        std::cout << "error in recvLISTprint" << std::endl;
    }
    if (std::string(buffer) == "ERR\n"){
        std::cout << "NO email with given username exists" << std::endl;
        return;
    }
    parseClass().parseLIST(buffer);
    return;
}

// fragt den user nach dem Input wenn er READ angegeben hat
struct TextPreset READinput(struct TextPreset tp)
{
    std::cout << "Username: ";
    if(tp.username == ""){
        std::getline(std::cin, tp.username);
    }else{
        std::cout << tp.username << std::endl;
    }
    
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
    if(tp.username == ""){
        std::getline(std::cin, tp.username);
    }else{
        std::cout << tp.username << std::endl;
    }
    return tp;
}

// verarbeitet den Input vom user Also was er gewählt hat ob SEND, LIST, READ etc...
int userINPUTfindOpt(int clientSocket, std::string username)
{
    std::string input = "";
    struct TextPreset tpInput = resetTP({});
    tpInput.username = username;
    std::getline(std::cin, input);
    if (input.substr(0, 4) == "SEND")
    {
        tpInput = SENDInput(tpInput);

        tpInput = basicFunktions().calcINFOstring(tpInput, SEND);
        basicFunktions().sendINFOstring(clientSocket, tpInput);
        if (tpInput.packageNUM == 1){
            sendMESSclass().sendMESSstring(clientSocket, tpInput);
        }
        else{
            sendMESSclass().sendMESSstring_Packages(clientSocket, tpInput);
        }
        return SEND;
    }
    else if (input.substr(0, 4) == "READ")
    {
        tpInput = READinput(tpInput);
        tpInput = basicFunktions().calcINFOstring(tpInput, READ);
        int testLIST = basicFunktions().sendINFOstring(clientSocket, tpInput);
        if (testLIST == -1){
            std::cout << "Not enough Elements to LIST" << std::endl;
            return READ;
        }
        sendIPREAD(tpInput, clientSocket);
        char buffer[1024] = {0};
        int errRCV = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[errRCV] = '\0';
        if (errRCV == -1){
            std::cout << "Error in READ string" << std::endl;
            send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
        }

        tpInput = parseClass().parseINFO(tpInput, std::string(buffer));

        send(clientSocket, "OK\n", sizeof("OK\n"), 0);
        if (tpInput.packageNUM == 1){
            initializeClass().initializeSENDSAVEClient(tpInput, clientSocket);
        }
        else{
            initializeClass().initializeSENDSAVE_PackagesClient(tpInput, clientSocket);
        }
        return READ;
    }
    else if (input.substr(0, 4) == "LIST")
    {
        tpInput = LISTinput(tpInput);
        tpInput = basicFunktions().calcINFOstring(tpInput, LIST);
        int testLIST = basicFunktions().sendINFOstring(clientSocket, tpInput);
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
        tpInput = basicFunktions().calcINFOstring(tpInput, DEL);
        int testLIST = basicFunktions().sendINFOstring(clientSocket, tpInput);

        if (testLIST == -1){std::cout << "Not enough Elements to LIST" << std::endl;return DEL;}
        sendIPREAD(tpInput, clientSocket);

        char buffer[1024] = {0};
        int errRCV = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[errRCV] = '\0';

        if(errRCV == -1){std::cout << "error in DEL recv" << std::endl;return DEL;}

        if(std::string(buffer) == "ERR\n"){
            std::cout << "item couldnt be deleted" << std::endl;
        }else if(std::string(buffer) == "OK\n"){
            std::cout << "item deleted" << std::endl;
        }

    }
    else if (input.substr(0, 4) == "QUIT")
    {
        tpInput = basicFunktions().calcINFOstring(tpInput, QUIT);
        int testLIST = basicFunktions().sendINFOstring(clientSocket, tpInput);
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
    std::string username = "";
    do{
    std::cout << "Enter Username: " << std::endl;
    getline(std::cin, username); 
    }while(username == "EMPTYSTRINGUSERNAME");

    std::cout << "listening on Port: " << argv[2] << std::endl;

    // Receive data from the serve
    std::cout << basicFunktions().recvFunctBasic(clientSocket) << std::endl;
    if(username == ""){username = "EMPTYSTRINGUSERNAME";}
    basicFunktions().sendFunctBasic(clientSocket, username);
    int startINPUT;
    if(username == "EMPTYSTRINGUSERNAME"){username = "";}

    do
    {
        std::cout << "\n --------- Open Terminal ----------" << std::endl;
        std::cout << "[SEND] [READ] [LIST] [DEL] [QUIT]" << std::endl;

        startINPUT = userINPUTfindOpt(clientSocket, username);

    } while (startINPUT != QUIT);

    std::cout << "quiting in prozess" << std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}