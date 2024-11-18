#ifndef USERINPUTCLASS_H
#define USERINPUTCLASS_H

#pragma once

#include "../headers/PresetStruct.h"

class userInputClass
{
public:
    userInputClass();
    ~userInputClass();
    TEXTpreset SENDInput(LOGINpreset lp);
    std::string READInput(LOGINpreset lp);
private:

};

#endif