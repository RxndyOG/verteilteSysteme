#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <utility>
#include <cmath>

#include "ServerClass.h"
#include "ErrorHandling.h"
#include "FileHandling.h"
#include "txtPreset.h"
#include "BasicSocketFunction.h"

void PrintUsage(char *argv[]){
    std::cout << "Usage: " << argv[0] << " <PORT> <Directory>" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        PrintUsage(argv);
        return -1;
    }
    
    ServerClass* server = new ServerClass();
    ErrorHandling error;
    FileHandling files;

    /*
    //wird als file saving benutzt momentan noch im testen
    txtPreset txt;
    txt.fileLocal = "data/test.txt";
    txt.uid = 0;
    txt.mailID = 0;
    txt.username = "test";
    txt.sender = "test";
    txt.subject = "test";
    txt.text = "test";

    files.saveToTXT(txt);
    // ende file saving test

    */

    server->CreateSocket();
    int err = server->BindSocket(atoi(argv[1]));    //methode der ServerClass klasse. Binded die Parameter zum socket

    // basic error handeling
    if(err != 0){
        error.CommonError(err);
        close(server->GetServerSocket());
        delete server;
        return err;
    }

    err = server->ListenSockets(argv[1]);   

    if(err != 0){
        error.CommonError(err);
        close(server->GetServerSocket());
        delete server;
        return err;
    }

    err = server->AcceptSocket();

    if(err != 0){
        error.CommonError(err);
        close(server->GetServerSocket());
        delete server;
        return err; 
    }

    err = server->UserIP();
    
    int cancell = 0;
    do  //endlosschleife zum bekommen des user inputs
    {
        std::string arg = BasicSocketFunction().recvFunctBasic(server->GetClientSocket());  //recvFunctBasic ist eine einfache funktion zum bekommen des user gesendeten contents
        cancell = BasicSocketFunction().recvParse(arg, server->GetClientSocket());                                     //recvParse parsed den gerade bekommenen string des users. Returned INT als ENUM bei return QUIT wird der server geschlossen.

    } while (cancell != QUIT);      // kann für multithreading nicht so weiter gehen muss angepasst werden

    std::cout << "client closed server" << std::endl;

    close(server->GetClientSocket());
    close(server->GetServerSocket());
    delete server;

    return 0;
}