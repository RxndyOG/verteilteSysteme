#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cmath>
#include <array>

#include "ErrorHandling.h"
#include "txtPreset.h"
#include "BasicSocketFunction.h"
#include "FileHandling.h"

BasicSocketFunction::BasicSocketFunction()
{

    argumentParse["SEND"] = [this](txtPreset *tp)
    { tp->ip.argument = SEND; std::cout << "SEND from Client" << std::endl; return SEND; };
    argumentParse["READ"] = [this](txtPreset *tp)
    { tp->ip.argument = READ; std::cout << "READ from Client" << std::endl; return READ; };
    argumentParse["LIST"] = [this](txtPreset *tp)
    { tp->ip.argument = LIST; std::cout << "LIST from Client" << std::endl; return LIST; };
    argumentParse["QUIT"] = [this](txtPreset *tp)
    { tp->ip.argument = QUIT; std::cout << "QUIT from Client" << std::endl; return QUIT; };
    argumentParse["DEL "] = [this](txtPreset *tp)
    { tp->ip.argument = DEL; std::cout << "DEL from Client" << std::endl; return DEL; };
}

BasicSocketFunction::~BasicSocketFunction()
{
}

void BasicSocketFunction::safeSENDInFile(txtPreset *tp)
{
    FileHandling files;
    tp->fileLocal.append("/");
    tp->fileLocal.append(tp->ip.username);
    tp->fileLocal.append(".txt");
    files.saveToTXT(*tp);
}

void BasicSocketFunction::parseMsg(txtPreset *tp, int socket)
{
    std::string completeMessage;
    while (tp->ip.packageNUM > 0)
    {
        std::cout << "test 1" << std::endl;
        std::string arg = recvFunctBasic(socket);
        std::cout << "test 2" << std::endl;
        completeMessage += arg;
        tp->ip.packageNUM--;
    }

    std::vector<std::string> argArray;
    std::string tmpString;

    for (char ch : completeMessage)
    {
        if (ch == '\n')
        {
            argArray.push_back(tmpString);
            tmpString.clear();
        }
        else
        {
            tmpString += ch;
        }
    }

    if (!tmpString.empty())
    {
        argArray.push_back(tmpString);
    }

    for (int j = 0; j < (int)argArray.size(); j++)
    {
        switch (j)
        {
        case 0:
            tp->sender = argArray[0];
            break;
        case 1:
            tp->subject = argArray[1];
            break;
        default:
            tp->text.append(argArray[j] + "\n");
            break;
        }
    }
}

void BasicSocketFunction::parseInfoString(txtPreset *tp)
{
    std::array<std::string, 5> argArray = {};
    size_t n = 0;
    std::string tmpString;

    for (char ch : tp->ip.infoString)
    {
        if (ch == '\n')
        {
            if (n < argArray.size())
            {
                argArray[n++] = tmpString;
                
            }
            tmpString.clear();
        }
        else
        {
            tmpString += ch;
        }
    }

    if (n < 4)
    {
        std::cerr << "Error: Incomplete infoString format. Expected 4 lines, got " << n << std::endl;
        return;
    }
    try
    {
        tp->ip.argument = std::stoi(argArray[0]);
        tp->ip.textLength = std::stoi(argArray[1]);
        tp->ip.packageLength = std::stoi(argArray[2]);
        tp->ip.packageNUM = std::stoi(argArray[3]);
        tp->ip.username = argArray[4];
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Error: Invalid argument in infoString, stoi conversion failed - " << e.what() << std::endl;
        return;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Error: Out of range error in infoString, stoi conversion failed - " << e.what() << std::endl;
        return;
    }
}

std::string BasicSocketFunction::recvFunctBasic(int socket)
{

    char buffer[_BlockSize] = {};
    int errRecv = recv(socket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\0';
    std::cout << "RECV: " << errRecv << std::endl;
    if (errRecv == -1)
    {
        std::cout << "error happened during recv from server" << std::endl;
    }
    /*
       if (buffer == "ERR\n")
       {
           std::cout << "error in rcv" << std::endl;
       }
       */
    return buffer;
}

void BasicSocketFunction::infoStringCalc(txtPreset *tp)
{
    tp->ip.textLength = tp->sender.size() + tp->subject.size() + tp->text.size() + 0;
    tp->ip.packageLength = _BlockSize;
    tp->ip.packageNUM = std::ceil(tp->ip.textLength / tp->ip.packageLength) + 1;
    tp->ip.infoString = std::to_string(1) + "\n" + std::to_string(tp->ip.textLength) + "\n" + std::to_string(tp->ip.packageLength) + "\n" + std::to_string(tp->ip.packageNUM) + "\n" + tp->username + "\n";
}

int BasicSocketFunction::recvParseClient(int socket, txtPreset *tp){

    std::cout << tp->ip.packageNUM << std::endl;
    parseMsg(tp, socket);
    std::string text = tp->sender + tp->subject + tp->text;
    return 0;
}

int BasicSocketFunction::recvParse(std::string arg, int socket, txtPreset *tp)
{

    auto it = argumentParse.find(arg.substr(0, 4));
    if (it != argumentParse.end())
    {
        tp->ip.infoString = recvFunctBasic(socket);
        sendFunctBasic(socket, "OK\n");
        parseInfoString(tp);
        int result = it->second(tp);
        int currentPosition = 0;
        int currentBlockSize = 0;
        switch (result)
        {
        case SEND:
            parseMsg(tp, socket);
            safeSENDInFile(tp);
            break;
        case READ:
            tp->fileLocal.append("/");
            tp->fileLocal.append(tp->ip.username);
            tp->fileLocal.append(".txt");
            tp->text = FileHandling().readFileToString(tp->fileLocal);
            infoStringCalc(tp);
            sendFunctBasic(socket, tp->ip.infoString);
            while (tp->ip.packageNUM > 0)
            {
                currentBlockSize = std::min(1024, tp->ip.textLength - currentPosition);
                std::string block = tp->text.substr(currentPosition, currentBlockSize);
                BasicSocketFunction().sendFunctBasic(socket, block);
                currentPosition += currentBlockSize;
                tp->ip.packageNUM--;
            }
            break;
        }

        return result;
    }
    else
    {
        std::cout << "COMMENT from Client" << std::endl;

        return COMMENT;
    }
}

void BasicSocketFunction::sendFunctBasic(int socket, std::string arg)
{
    std::cout << arg << std::endl;
    int err = send(socket, arg.c_str(), arg.size(), 0);
    std::cout <<"SEND: " << err << std::endl;
    if (err == -1)
    {
        std::cout << "error in send" << std::endl;
    }
}
