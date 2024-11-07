#include "ErrorHandling.h"
#include <iostream>

ErrorHandling::ErrorHandling()
{

}

ErrorHandling::~ErrorHandling()
{

}

int ErrorHandling::CommonError(int err){
    switch(err){
        case 1:
            std::cerr << "Error in Socket Creation: ! ServerClass::CreateSocket() !" << std::endl;
            break;
        case -1:
            std::cerr << "Error in Socket Binding: ! ServerClass::BindSocket() !" << std::endl; 
            break;
        case -2:
            std::cerr << "Error in Socket Listen: ! ServerClass::ListenSockets() !" << std::endl;
            break;
        case -3:
            std::cerr << "Error in Socket Accept: ! ServerClass::AcceptSocket() !" << std::endl;
            break;
    }
    
    return err;
}