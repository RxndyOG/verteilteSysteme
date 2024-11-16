#ifndef USERINPUTCLASS_H
#define USERINPUTCLASS_H

#pragma once

#include "../headers/PresetsStruct.h"

class userInputClass
{
public:
    userInputClass();
    ~userInputClass();
    TextPreset SENDInput(TextPreset tp);
    TextPreset READinput(TextPreset tp);
    TextPreset LISTinput(TextPreset tp);

private:

};

#endif