#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>
#include <iostream>

#include "ClientClass.h"
#include "UserInputClass.h"
#include "ErrorHandling.h"
#include "txtPreset.h"
#include "BasicSocketFunction.h"

std::string convertToLowercase(const std::string &str){
    std::string result = "";
    for (char ch : str){result += tolower(ch);}
    return result;
}

void PrintUsage(char *argv[]){
    std::cout << "Usage: " << argv[0] << " <IP-Adresse> <Port>" << std::endl;
}

int getUserInput(ClientClass* client){
    int i = UserInputClass().ChooseInput(client);
    return i;
}

int main(int argc, char *argv[]){

    if (argc < 3){
        PrintUsage(argv);
        return -1;
    }

    std::string ipAddress = argv[1];
    if (convertToLowercase(ipAddress) == "localhost"){ipAddress = "127.0.0.1";}

    ClientClass* client = new ClientClass();
    ErrorHandling error;

    int err = client->CreateSocket();

    if(err != 0){
        error.CommonError(err);
        close(client->GetClientSocket());
        delete client;
        return err;
    }

    err = client->ConnectSocket(atoi(argv[2]), ipAddress.c_str());

    if(err != 0){
        error.CommonError(err);
        close(client->GetClientSocket());
        delete client;
        return err;
    }

    std::cout << "listening on Port: " << argv[2] << std::endl;

    std::string recv = BasicSocketFunction().recvFunctBasic(client->GetClientSocket());

    int startINPUT = 0;
    do
    {
        std::cout << "\n --------- Open Terminal ----------" << std::endl;
        std::cout << "[SEND] [READ] [LIST] [DEL] [QUIT]" << std::endl;
        startINPUT = getUserInput(client);


    } while (startINPUT != QUIT);

    std::cout << "quiting in prozess" << std::endl;

    close(client->GetClientSocket());

    delete client;

    return 0;
}