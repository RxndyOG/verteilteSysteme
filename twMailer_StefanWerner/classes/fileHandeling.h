#ifndef FILEHANDELING_H
#define FILEHANDELING_H

#pragma once

#include "../headers/PresetStruct.h"
#include <vector>

class fileHandeling
{
public:
    fileHandeling();
    ~fileHandeling();
    std::vector<TEXTpreset> READ_from_File(std::string local, std::string username);
    void SAFE_to_File(std::string local, std::string username, TEXTpreset tp);
    void clearFile(std::string local, std::string username);
private:

};

#endif