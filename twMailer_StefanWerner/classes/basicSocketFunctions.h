#ifndef BASICSOCKETFUNCTIONS_H
#define BASICSOCKETFUNCTIONS_H

#pragma once
#include "../headers/PresetStruct.h"

class basicSocketFunctions
{
public:
    basicSocketFunctions();
    ~basicSocketFunctions();
    INFOpreset SEND_and_CALC_infoString(TEXTpreset tp, int socked_fd);
    std::string RCV_and_PARSE_serverResponse(int socked_fd);
    recvReturn recvFunctBasic(int client_socket);
private:

};

#endif