#ifndef USERINPUTCLASS_H
#define USERINPUTCLASS_H

#pragma once

#include <unordered_map>
#include <functional>
#include "txtPreset.h"

class UserInputClass
{
public:
    UserInputClass();
    ~UserInputClass();

    std::string UserProfile();

    int sendInput(txtPreset* tp);
    int ChooseInput(txtPreset* tp, ClientClass* client);

private:

    std::unordered_map<std::string, std::function<int(txtPreset*)>> argumentClient = {};

};

#endif