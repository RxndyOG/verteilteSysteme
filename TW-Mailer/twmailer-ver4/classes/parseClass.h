#ifndef PARSECLASS_H
#define PARSECLASS_H

#pragma once

#include "../headers/PresetsStruct.h"

class parseClass
{
public:
    parseClass();
    ~parseClass();

    void parseLIST(std::string text);
    TextPreset parseINFO(TextPreset tp, std::string info);
    TextPreset parseREAD(TextPreset tp, std::string text);
    TextPreset parseSEND(TextPreset tp, std::string sendMESS);
    TextPreset parseREADServer(TextPreset tp, std::string text);

private:

};

#endif