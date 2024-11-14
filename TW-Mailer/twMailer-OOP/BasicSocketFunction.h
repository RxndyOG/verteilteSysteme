#ifndef BASICSOCKETFUNCTION_H
#define BASICSOCKETFUNCTION_H

#pragma once

#include <unordered_map>
#include <functional>
#include "txtPreset.h"

class BasicSocketFunction
{
public:
    BasicSocketFunction();
    ~BasicSocketFunction();

    std::string recvFunctBasic(int i);

    void safeSENDInFile(txtPreset *tp);
    void parseMsg(txtPreset* tp,int socket);
    void parseInfoString(txtPreset* tp);
    void infoStringCalc(txtPreset* tp);
    int recvParse(std::string arg, int socket, txtPreset *tp);
    int recvParseClient(int socket, txtPreset *tp);

    void sendFunctBasic(int i, std::string arg);

private:

    std::unordered_map<std::string, std::function<int(txtPreset*)>> argumentParse = {};
};

#endif