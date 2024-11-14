#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#pragma once

#include "txtPreset.h"

class FileHandling
{
public:
    FileHandling();
    ~FileHandling();

    std::string readFileToString(const std::string&);
    int saveToTXT(txtPreset txt);

private:

};

#endif