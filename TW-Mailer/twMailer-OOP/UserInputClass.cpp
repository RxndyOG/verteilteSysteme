#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

#include "txtPreset.h"
#include "BasicSocketFunction.h"
#include "ClientClass.h"
#include "UserInputClass.h"


UserInputClass::UserInputClass()
{

    argumentClient["SEND"] = [this](txtPreset * tp){ tp->ip.argument = SEND; return this->sendInput(tp); };
    argumentClient["READ"] = [this](txtPreset * tp){ tp->ip.argument = READ; return READ; };
    argumentClient["LIST"] = [this](txtPreset * tp){ tp->ip.argument = LIST; return LIST; };
    argumentClient["QUIT"] = [this](txtPreset * tp){ tp->ip.argument = QUIT; return QUIT; };
    argumentClient["DEL "] = [this](txtPreset * tp){ tp->ip.argument = DEL; return DEL; };

}

UserInputClass::~UserInputClass()
{

}

int UserInputClass::UserProfile(txtPreset* tp){



    return 0;
}


int UserInputClass::sendInput(txtPreset* tp){
    tp->ip.argument = SEND;
    std::cout << "Sender: " << std::endl;
    std::getline(std::cin, tp->sender);
    std::cout << "Subject: " << std::endl;
    std::getline(std::cin, tp->subject);
    std::cout << "Message: " << std::endl;
    std::string tmpString;
    do{
        tmpString = {};
        std::getline(std::cin, tmpString);
        if(tmpString == "."){break;}
        tp->text.append(tmpString);
        tp->text.append("\n");
    }while(true);
    return SEND;
}

int UserInputClass::ChooseInput(txtPreset* tp, ClientClass* client){

    std::string input = "";
    std::getline(std::cin, input);

    auto it = argumentClient.find(input.substr(0,4));
    if (it != argumentClient.end()) {
        int result = it->second(tp);
        BasicSocketFunction().sendFunctBasic(client->GetClientSocket(), input.substr(0,4));
        BasicSocketFunction().infoStringCalc(tp);
        BasicSocketFunction().sendFunctBasic(client->GetClientSocket(), tp->ip.infoString);
        return result;
    }else{
        BasicSocketFunction().sendFunctBasic(client->GetClientSocket(), input);
        return COMMENT;
    }
    tp->ip = infoPreset(); ///// aufgehört weiter machen
}