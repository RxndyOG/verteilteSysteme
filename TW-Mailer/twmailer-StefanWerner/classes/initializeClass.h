#ifndef INITIALIZECLASS_H
#define INITIALIZECLASS_H

#pragma once
#include <vector>
#include "../headers/PresetsStruct.h"

class initializeClass
{
public:
    initializeClass();
    ~initializeClass();
    
    void initializeSENDSAVE(TextPreset tp, int clientSocket, std::vector<TextPreset> &n);
    void initializeSENDSAVE_Packages(TextPreset tp, int clientSocket, std::vector<TextPreset> &n);
    void initializeSENDSAVEClient(TextPreset tp, int clientSocket);
    void initializeSENDSAVE_PackagesClient(TextPreset tp, int clientSocket);
    void initializeREAD(TextPreset tp, int clientSocket, std::vector<TextPreset> &n);
    void initializeDEL(TextPreset tp, int clientSocket, std::vector<TextPreset> &n);
private:

};

#endif