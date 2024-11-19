#include "userInputClass.h"

#include "../headers/PresetStruct.h"

#include <iostream>
#include <termios.h>
#include <unistd.h>

userInputClass::userInputClass()
{
}

userInputClass::~userInputClass()
{
}

std::string getPassword() {
    std::string password;
    char ch;
    struct termios oldt, newt;

    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;  
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); 

    while (true) {
        ch = getchar();  
        if (ch == '\n')
            break;

        password.push_back(ch);  
        std::cout << '*';  
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    std::cout << std::endl;
    return password;
}

LOGINpreset userInputClass::LOGINinput()
{
    LOGINpreset lp;
    std::cout << "Enter username: ";
    std::getline(std::cin, lp.username);
    std::cout << "Enter PWD: ";
    lp.pwd = getPassword();

    return lp;
}

std::string userInputClass::READInput(LOGINpreset lp)
{
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
    do
    {
        std::cout << "subject (Max: 80 chars): ";
        std::getline(std::cin, tp.subject);
        if (tp.subject.size() > 80)
        {
            std::cout << "You entered more than 80 Chars !\n"
                      << "Do you want to enter another subject or are you ok with your Subject being cut down?\n"
                      << "[[y] Yes cut down / [n] No let me redo it]" << std::endl;
            std::string inputError;
            std::getline(std::cin, inputError);
            if (inputError.substr(0, 1) == "y" || inputError.substr(0, 4) == "Y")
            {
                tp.subject = tp.subject.substr(0,80);
                break;
            }
        }
    } while (tp.subject.size() > 80);
    std::string currentMESS = "";
    std::cout << "message: ";
    do
    {
        currentMESS.clear();
        std::getline(std::cin, currentMESS);
        if (currentMESS == ".")
        {
            break;
        }
        tp.message += currentMESS;
        tp.message.append("\n");
    } while (currentMESS != ".");
    return tp;
}