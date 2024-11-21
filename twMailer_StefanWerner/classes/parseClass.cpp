#include "parseClass.h"

#include "../headers/PresetStruct.h"
#include <sstream>

parseClass::parseClass()
{

}

parseClass::~parseClass()
{

}

INFOpreset parseClass::parseINFO(std::string infoString)   //parsed die info string
{                                       // TextLength\nPackageNum\n
    INFOpreset ip;

    std::string txtLen, nPack;
    std::istringstream iss(infoString);
    std::getline(iss, txtLen);
    std::getline(iss, nPack);
    ip.textLength = std::stoi(txtLen);
    ip.numPack = std::stoi(nPack);
    return ip;
}

TEXTpreset parseClass::parseSEND(const std::string &sendString) // prased den SEND string
{                                                    // Sender\nSubject\nMessage
    TEXTpreset tp;
    std::istringstream iss(sendString);
    std::string line;

    std::getline(iss, tp.receiver);
    std::getline(iss, tp.subject);

    tp.message.clear();
    while (std::getline(iss, line))
    {
        if (!tp.message.empty())
        {
            tp.message += "\n"; 
        }
        tp.message += line;
    }

    return tp;
}

LOGINpreset parseClass::parseLOGIN(std::string loginString) //parsed den LOGIN string
{                                       // Username\nPassword\n
    LOGINpreset lp;

    std::string username, pwd;

    std::istringstream iss(loginString);

    std::getline(iss, username); 
    std::getline(iss, pwd);      

    lp.username = username;
    lp.pwd = pwd;
    return lp;
}
