#include <fstream>
#include <iostream>
#include <string>
#include "FileHandling.h"
#include "txtPreset.h"

FileHandling::FileHandling()
{

}

FileHandling::~FileHandling()
{

}

int FileHandling::saveToTXT(txtPreset txt){

    std::ofstream outFile(txt.fileLocal, std::ios::app);

    if (outFile.is_open()) {

        outFile << "{\n";
        outFile << txt.uid <<"\n";
        outFile << txt.username <<"\n";
        outFile << txt.mailID <<"\n";
        outFile << txt.sender <<"\n";
        outFile << txt.subject <<"\n";
        outFile << txt.text <<"\n";
        outFile << "}\n";
        outFile.close();
    } else {
        std::cerr << "Die Datei konnte nicht geöffnet werden!" << std::endl;
    }

    return 0;
}