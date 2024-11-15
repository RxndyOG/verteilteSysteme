#ifndef BASICFUNKTIONS_H
#define BASICFUNKTIONS_H

#pragma once

#include "../headers/PresetsStruct.h"

class basicFunktions
{
public:
    basicFunktions();
    ~basicFunktions();

    TextPreset calcINFOstring(TextPreset tp, int type);
    int sendINFOstring(int clientSocket, TextPreset tp);
    std::string recvFunctBasic(int clientSocket);
    void sendFunctBasic(int clientSocket, std::string text);
private:

};

#endif