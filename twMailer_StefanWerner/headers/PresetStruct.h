#ifndef PRESETSTRUCT_H
#define PRESETSTRUCT_H

#include <string>

#define _BLOCK_SIZE 1024

struct LOGINpreset
{
    std::string username = "";
    std::string pwd = "";
};

struct TEXTpreset
{
    std::string sender = "";
    std::string subject = "";
    std::string message = "";
};

struct INFOpreset
{
    int textLength = 0;
    int numPack = 0;
};

struct recvReturn{
    std::string buffer = {};
    int err = 0;
};

#endif