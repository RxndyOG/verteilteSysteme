#ifndef TXTPRESET_H
#define TXTPRESET_H

#include <string>

struct txtPreset {
    int uid;
    int mailID;
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