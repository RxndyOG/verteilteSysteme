#include "FileHandeling.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "../headers/PresetsStruct.h"

FileHandeling::FileHandeling()
{

}

FileHandeling::~FileHandeling()
{

}

std::string FileHandeling::readFileToString(std::string file, std::string username) {
    std::string fileLocal = file + "/" + username + ".txt";
    std::ifstream inFile(fileLocal);

    if (!inFile.is_open()) {
        std::cerr << "Die Datei konnte nicht geöffnet werden!" << std::endl;
        return "";
    }

    std::ostringstream ss;
    ss << inFile.rdbuf();  // Dateiinhalt in den String-Stream lesen
    inFile.close();

    return ss.str();  // String aus dem String-Stream zurückgeben
}

void FileHandeling::clearFile(TextPreset txt) {
    txt.fileLocal = txt.fileLocal + "/" + txt.username + ".txt";
    FILE* file = fopen(txt.fileLocal.c_str(), "w"); // Öffnet die Datei im Schreibmodus
    if (file == nullptr) {
        perror("Fehler beim Öffnen der Datei");
    } else {
        fclose(file); // Schließt die Datei
    }
}

std::vector<TextPreset> FileHandeling::parseFile(std::string arg){
    std::vector<TextPreset> parsedMessages;
    std::istringstream stream(arg);
    std::string line;
    TextPreset currentMessage;

    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        if (line == "{") {
            currentMessage = TextPreset();
        } else if (line == "}") {
            parsedMessages.push_back(currentMessage);
        } else {
            if (currentMessage.sender.empty()) {
                currentMessage.sender = line;
            } else if (currentMessage.subject.empty()) {
                currentMessage.subject = line;
            } else {
                currentMessage.text += line + " ";
            }
        }

        currentMessage.username = currentMessage.sender;

    }

    return parsedMessages;
}

int FileHandeling::saveToTXT(TextPreset txt){
    txt.fileLocal = txt.fileLocal + "/" + txt.username + ".txt";

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