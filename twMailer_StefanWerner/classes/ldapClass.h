#ifndef LDAPCLASS_H
#define LDAPCLASS_H

#pragma once

#include <iostream>

class ldapClass
{
public:
    ldapClass();
    ~ldapClass();
    int connectToLDAP(std::string username, std::string pwd);
private:

};

#endif