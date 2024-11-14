#include <iostream>

#include "parseClass.h"
#include "../headers/PresetsStruct.h"

parseClass::parseClass()
{

}

parseClass::~parseClass()
{

}

TextPreset parseClass::parseREADServer(struct TextPreset tp, std::string sendMESS)
{
    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < sendMESS.size(); i++)
    {
        if (sendMESS[i] == '\n')
        {
            switch (j)
            {
            case 0:
                tp.username = parseTemp;
                break;
            case 1:
                tp.ID = std::stoi(parseTemp);
                break;
            default:
                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += sendMESS[i];
        }
    }
    return tp;
}

TextPreset parseClass::parseREAD(TextPreset tp, std::string text)
{
    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < text.size(); i++)
    {
        if (text[i] == '\n')
        {
            switch (j)
            {
            case 0:
                tp.argument = parseTemp;
                break;
            case 1:
                tp.sender = parseTemp;
                break;
            case 2:
                tp.subject = parseTemp;
                break;
            default:
                tp.text += parseTemp;
                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += text[i];
        }
    }

    return tp;
}

void parseClass::parseLIST(std::string text)
{

    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < text.size(); i++)
    {
        if (text[i] == '\n')
        {
            switch (j % 4)
            {
            case 0:
                std::cout << "ID: " << parseTemp << std::endl;
                break;
            case 1:
                std::cout << "SENDER: " << parseTemp << std::endl;
                break;
            case 2:
                std::cout << "SUBJECT: " << parseTemp << std::endl;
                break;
            case 3:
                std::cout << "TEXT (first 10): " << parseTemp << std::endl;
                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += text[i];
        }
    }
}

TextPreset parseClass::parseSEND(TextPreset tp, std::string sendMESS)
{
    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < sendMESS.size(); i++)
    {
        if (sendMESS[i] == '\n')
        {
            switch (j)
            {
            case 0:
                tp.argument = parseTemp;
                break;
            case 1:
                tp.sender = parseTemp;
                break;
            case 2:
                tp.subject = parseTemp;
                break;
            default:
                tp.text += parseTemp;
                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += sendMESS[i];
        }
    }

    return tp;
}

TextPreset parseClass::parseINFO(TextPreset tp, std::string info){
    tp.infoString = info;
    std::string parseTemp = "";
    int j = 0;
    for (long unsigned int i = 0; i < info.size(); i++)
    {
        if (info[i] == '\n')
        {
            switch (j)
            {
            case 0:
                tp.type = std::stoi(parseTemp);
                break;
            case 1:
                tp.packageNUM = std::stoi(parseTemp);
                break;
            case 2:
                tp.length = std::stoi(parseTemp);
                break;
            default:

                break;
            }
            parseTemp = "";
            j++;
        }
        else
        {
            parseTemp += info[i];
        }
    }

    return tp;
}