#include "parseClass.h"

#include "../headers/PresetStruct.h"
#include <sstream>

parseClass::parseClass()
{

}

parseClass::~parseClass()
{

}

INFOpreset parseClass::parseINFO(std::string infoString)
{
    INFOpreset ip;

    std::string txtLen, nPack;
    std::istringstream iss(infoString);
    std::getline(iss, txtLen);
    std::getline(iss, nPack);
    ip.textLength = std::stoi(txtLen);
    ip.numPack = std::stoi(nPack);
    return ip;
}

TEXTpreset parseClass::parseSEND(const std::string &sendString)
{
    TEXTpreset tp;
    std::istringstream iss(sendString);
    std::string line;

    // Sender auslesen
    std::getline(iss, tp.sender);

    // Subject auslesen
    std::getline(iss, tp.subject);

    // Restlichen Text als Body zusammenfassen
    tp.message.clear();
    while (std::getline(iss, line))
    {
        if (!tp.message.empty())
        {
            tp.message += "\n"; // Text wieder mit Zeilenumbruch trennen
        }
        tp.message += line;
    }

    return tp;
}

LOGINpreset parseClass::parseLOGIN(std::string loginString)
{
    LOGINpreset lp;
    // Variablen für username und pwd
    std::string username, pwd;
    // Stringstream zum Parsen verwenden
    std::istringstream iss(loginString);
    // Zeilenweise lesen
    std::getline(iss, username); // Liest die erste Zeile
    std::getline(iss, pwd);      // Liest die zweite Zeile

    lp.username = username;
    lp.pwd = pwd;
    return lp;
}
