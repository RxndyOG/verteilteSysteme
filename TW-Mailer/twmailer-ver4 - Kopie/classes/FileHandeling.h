#ifndef FILEHANDELING_H
#define FILEHANDELING_H

#pragma once

#include <string>
#include <vector>
#include "../headers/PresetsStruct.h"

class FileHandeling
{
public:
    FileHandeling();
    ~FileHandeling();

    std::string readFileToString(std::string file, std::string username);
    int saveToTXT(TextPreset txt);
    std::vector<TextPreset> parseFile(std::string arg);
    void clearFile(TextPreset txt);

private:

};

#endif