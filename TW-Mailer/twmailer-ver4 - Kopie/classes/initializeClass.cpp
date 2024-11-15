#include "initializeClass.h"

#include <vector>
#include <iostream>
#include <sys/socket.h>

#include "../headers/PresetsStruct.h"
#include "parseClass.h"
#include "sendMESSclass.h"
#include "basicFunktions.h"
#include "FileHandeling.h"

initializeClass::initializeClass()
{

}

initializeClass::~initializeClass()
{

}

std::string recvINIT(int clientSocket){
    char buffer[1024] = {0};
    int errRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRcv] = '\0';
    if (errRcv == -1){std::cout << "Error in initialize single pack" << std::endl;}
    return buffer;
}

void initializeClass::initializeREAD(TextPreset tp, int clientSocket, std::vector<TextPreset> &n)
{

    std::string buffer = recvINIT(clientSocket);
    tp = parseClass().parseREADServer(tp, buffer);

    if (static_cast<long unsigned int>(tp.ID) > n.size()){
        std::cout << "Error in initializeREAD ID not existing" << std::endl;
        return;
    }
    else{
        if (tp.username == n[tp.ID].sender){
            tp.argument = "READ";
            tp.sender = n[tp.ID].sender;
            tp.subject = n[tp.ID].subject;
            tp.text = n[tp.ID].text;
        }
    }

    std::string READstring = "";
    READstring = READstring + tp.argument + "\n" + tp.sender + "\n" + tp.subject + "\n" + tp.text + "\n";

    tp = basicFunktions().calcINFOstring(tp, SEND);
    basicFunktions().sendINFOstring(clientSocket, tp);
    
    if (tp.packageNUM == 1)
    { sendMESSclass().sendMESSstring(clientSocket, tp);}
    else
    { sendMESSclass().sendMESSstring_Packages(clientSocket, tp);}
}

void initializeClass::initializeDEL(TextPreset tp, int clientSocket, std::vector<TextPreset> &n)
{
    char buffer[1024] = {0};
    int errRCV = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRCV] = '\0';
    if (errRCV == -1){    
        std::cout << "Error in recvLISTstring" << std::endl;
        send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
    }
    
    tp = parseClass().parseREAD(tp, std::string(buffer));

    if (static_cast<long unsigned int>(tp.ID) <= n.size()){
        if (tp.username == n[tp.ID].username){
            n.erase(n.begin() + tp.ID);
            std::cout << "email deleted" << std::endl;
            send(clientSocket, "OK\n", sizeof("OK\n"), 0);
            FileHandeling().clearFile(tp);
            for(auto &ch : n){
                FileHandeling().saveToTXT(ch);
            }

            return;
        }
        else{
            std::cout << "email couldnt be deleted. SENDER not correct" << std::endl;
            send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
            return;
        }
    }
    else{
        std::cout << "email couldnt be deleted. ID not correct" << std::endl;
        send(clientSocket, "ERR\n", sizeof("ERR\n"), 0);
        return;
    }



}

void initializeClass::initializeSENDSAVE(TextPreset tp, int clientSocket, std::vector<TextPreset> &n)
{
    std::string buffer = recvINIT(clientSocket);
    tp = parseClass().parseSEND(tp, buffer);
    FileHandeling().saveToTXT(tp);
    n.push_back(tp);
}

std::string initPackages(TextPreset tp, int clientSocket){
    std::string completeMessage;
    int totalPackages = tp.packageNUM;
    int currentPackage = 1;
    while (currentPackage <= totalPackages){   
        char buffer[1024] = {0};
        int errRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[errRcv] = '\0';
        if (errRcv == -1){std::cerr << "Error in receiving package " << currentPackage << std::endl;}
        completeMessage.append(buffer, errRcv);
        ++currentPackage;
    }
    return completeMessage;
}

void initializeClass::initializeSENDSAVE_Packages(TextPreset tp, int clientSocket, std::vector<TextPreset> &n)
{
    std::string completeMessage = initPackages(tp, clientSocket);
    tp = parseClass().parseSEND(tp, completeMessage);
    FileHandeling().saveToTXT(tp);
    n.push_back(tp);
}

void outputTP(TextPreset tp){
    std::cout << "Sender: " << tp.sender << std::endl;
    std::cout << "Subject: " << tp.subject << std::endl;
    std::cout << "Message: " << tp.text << std::endl;
}

void initializeClass::initializeSENDSAVEClient(TextPreset tp, int clientSocket)
{
    std::string buffer = recvINIT(clientSocket);
    tp = parseClass().parseREAD(tp, buffer);
    outputTP(tp);
}

void initializeClass::initializeSENDSAVE_PackagesClient(TextPreset tp, int clientSocket)
{
    std::string completeMessage = initPackages(tp, clientSocket);
    tp = parseClass().parseREAD(tp, completeMessage);
    outputTP(tp);
}
