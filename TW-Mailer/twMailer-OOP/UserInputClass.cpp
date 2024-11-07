#include <iostream>
#include "txtPreset.h"
#include "BasicSocketFunction.h"
#include "ClientClass.h"
#include "UserInputClass.h"


UserInputClass::UserInputClass()
{

}

UserInputClass::~UserInputClass()
{

}
/**/
int UserInputClass::ChooseInput(ClientClass* client){

    std::string input = "";
    std::cin >> input;
    if(input.substr(0,4) == "SEND"){
        BasicSocketFunction().sendFunctBasic(client->GetClientSocket(),"SEND");
        return SEND;
    }
    if(input.substr(0,4) == "READ"){
        BasicSocketFunction().sendFunctBasic(client->GetClientSocket(),"READ");
        return READ;
    }
    if(input.substr(0,4) == "LIST"){
        BasicSocketFunction().sendFunctBasic(client->GetClientSocket(),"LIST");
        return LIST;
    }
    if(input.substr(0,4) == "QUIT"){
        BasicSocketFunction().sendFunctBasic(client->GetClientSocket(),"QUIT");
        return QUIT;
    }
    if(input.substr(0,3) == "DEL"){
        BasicSocketFunction().sendFunctBasic(client->GetClientSocket(),"DEL");
        return DEL;
    }

    BasicSocketFunction().sendFunctBasic(client->GetClientSocket(), input);
    return COMMENT;
}