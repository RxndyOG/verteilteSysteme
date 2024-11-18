#ifndef PARSECLASS_H
#define PARSECLASS_H

#pragma once

#include "../headers/PresetStruct.h"

class parseClass
{
public:
    parseClass();
    ~parseClass();
    LOGINpreset parseLOGIN(std::string loginString);
    TEXTpreset parseSEND(const std::string &sendString);
    INFOpreset parseINFO(std::string infoString);
private:

};

#endif