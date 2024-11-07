#ifndef BASICSOCKETFUNCTION_H
#define BASICSOCKETFUNCTION_H

#pragma once

class BasicSocketFunction
{
public:
    BasicSocketFunction();
    ~BasicSocketFunction();

    std::string recvFunctBasic(int i);
    int recvParse(std::string arg);

    void sendFunctBasic(int i, std::string arg);

private:

};

#endif