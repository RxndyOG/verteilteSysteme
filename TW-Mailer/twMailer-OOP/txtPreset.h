#ifndef TXTPRESET_H
#define TXTPRESET_H

#include <string>
#include <cstring>

#define _BlockSize 1024

struct infoPreset{
    int argument;
    int packageNUM;
    int packageLength;
    int textLength;
    int uid;
    std::string username;
    std::string infoString;

    infoPreset() 
        : argument(0), packageNUM(0), packageLength(0), 
          textLength(0), uid(0), username(""), infoString("") {}
};

struct txtPreset {
    infoPreset ip;
    int uid;
    int mailID;
    std::string fileLocal;
    std::string username;
    std::string sender;
    std::string subject;
    std::string text;
};

enum type
{
    SEND = 1,
    READ = 2,
    LIST = 3,
    DEL = 4,
    QUIT = 5,
    COMMENT = 0,
};


#endif // TXTPRESET_H