#include <fstream>
#include <iostream>
#include <string>
#include <sstream>


#include "FileHandling.h"
#include "txtPreset.h"

FileHandling::FileHandling()
{

}

FileHandling::~FileHandling()
{

}


std::string FileHandling::readFileToString(const std::string& fileName) {
    std::ifstream inFile(fileName);

    if (!inFile.is_open()) {
        std::cerr << "Die Datei konnte nicht geöffnet werden!" << std::endl;
        return "";
    }

    std::ostringstream ss;
    ss << inFile.rdbuf();  // Dateiinhalt in den String-Stream lesen
    inFile.close();

    return ss.str();  // String aus dem String-Stream zurückgeben
}

int FileHandling::saveToTXT(txtPreset txt){

    std::ofstream outFile(txt.fileLocal, std::ios::app);

    if (outFile.is_open()) {

        outFile << "{\n";
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