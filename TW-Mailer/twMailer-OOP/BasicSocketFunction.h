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

    void infoStringCalc(txtPreset* tp);
    int recvParse(std::string arg);

    void sendFunctBasic(int i, std::string arg);

private:

    std::unordered_map<std::string, std::function<int(txtPreset*)>> argumentParse = {};
};

#endif