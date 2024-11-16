#include "userInputClass.h"

#include "../headers/PresetsStruct.h"
#include <iostream>

userInputClass::userInputClass()
{

}

userInputClass::~userInputClass()
{

}

TextPreset userInputClass::SENDInput(TextPreset tp)
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

TextPreset userInputClass::READinput(TextPreset tp)
{
    std::cout << "Username: ";
    if(tp.username == ""){
        std::getline(std::cin, tp.username);
    }else{
        std::cout << tp.username << std::endl;
    }
    
    std::cout << "ID: ";
    std::string tempID = "";
    std::getline(std::cin, tempID);
    tp.ID = std::stoi(tempID);
    return tp;
}

TextPreset userInputClass::LISTinput(TextPreset tp)
{
    std::cout << "username: ";
    if(tp.username == ""){
        std::getline(std::cin, tp.username);
    }else{
        std::cout << tp.username << std::endl;
    }
    return tp;
}