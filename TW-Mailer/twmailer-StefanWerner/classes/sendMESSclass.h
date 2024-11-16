#ifndef SENDMESSCLASS_H
#define SENDMESSCLASS_H

#pragma once

#include "../headers/PresetsStruct.h"

class sendMESSclass
{
public:
    sendMESSclass();
    ~sendMESSclass();
    int sendMESSstring(int clientSocket,TextPreset tp);
    int sendMESSstring_Packages(int clientSocket, TextPreset tp);
private:

};

#endif