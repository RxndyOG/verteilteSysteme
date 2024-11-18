#include "userInputClass.h"

#include "../headers/PresetStruct.h"

#include <iostream>

userInputClass::userInputClass()
{

}

userInputClass::~userInputClass()
{

}

std::string userInputClass::READInput(LOGINpreset lp){
    TEXTpreset tp;
    std::cout << "Username: ";
    tp.sender = lp.username;
    std::cout << tp.sender << std::endl;
    std::cout << "ID: ";
    std::string id;
    std::getline(std::cin, id);
    return id;
}

TEXTpreset userInputClass::SENDInput(LOGINpreset lp)
{
    TEXTpreset tp;
    std::cout << "sender: ";
    tp.sender = lp.username;
    std::cout << tp.sender << std::endl;
    std::cout << "subject: ";
    std::getline(std::cin, tp.subject);
    std::string currentMESS = "";
    std::cout << "message: ";
    do
    {
        currentMESS.clear();
        std::getline(std::cin, currentMESS);
        if(currentMESS == "."){break;}
        tp.message += currentMESS;
        tp.message.append("\n");
    } while (currentMESS != ".");
    return tp;
}