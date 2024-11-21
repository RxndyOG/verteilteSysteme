#include "fileHandeling.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

fileHandeling::fileHandeling()
{

}

fileHandeling::~fileHandeling()
{

}

void fileHandeling::clearFile(std::string local, std::string username) {        // löscht die data file oder .data
    std::string filename = local + "/" + username + ".txt";
    std::ofstream file(filename, std::ios::out); 
    if (file.is_open()) {
        std::cout << "Die Datei \"" << filename << "\" wurde erfolgreich geleert.\n";
        file.close();
    } else {
        std::cerr << "Fehler beim Öffnen der Datei \"" << filename << "\" zum Leeren.\n";
    }
}

std::vector<TEXTpreset> fileHandeling::READ_from_File(std::string local, std::string username) // ließt die data file für messages
{
    std::string location = local + "/" + username + ".txt";
    std::ifstream inFile(location);

    std::vector<TEXTpreset> fileREAD;

    if (inFile.is_open())
    {
        std::string line;
        int i = 0;  
        TEXTpreset tp;

        while (getline(inFile, line))
        {
           
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        
            if (line.empty())
            {
                continue;
            }

            
            if (line == "[break/$TEXT$/$LOCAL$]")
            {
              
                if (!tp.message.empty())
                {
                    fileREAD.push_back(tp);
                }
                tp = TEXTpreset();  
                i = 0;              
                continue;
            }
            else if (i == 0)
            {
                tp.receiver = line;
            }
            else if (i == 1)
            {
                tp.subject = line;
            }
            else if (i >= 2)
            {
                tp.message.append(line + "\n"); 
            }

            i++; 
        }

        if (!tp.message.empty())
        {
            fileREAD.push_back(tp);
        }

        inFile.close();
        return fileREAD;
    }
    else
    {
        std::cout << "Die Datei konnte nicht geöffnet werden.\n";
    }

    return fileREAD;
}

int fileHandeling::readLoginFailures(std::string local, std::string username){ // ließt den integer in .data für die falsche eingabe
    std::string location = "." + local + "/" + username + ".txt";
    std::ifstream inFile(location);

    if (inFile.is_open())
    {
        std::string line;

        while (getline(inFile, line))
        {
            return std::stoi(line);     
        }
    }
    else
    {
        std::cout << "Die Datei konnte nicht geöffnet werden.\n";
    }
    return 0;
}

void fileHandeling::SAFE_to_FileLOGIN(std::string local, std::string username, int index) // speichert failed attempts
{
    std::ofstream outFile;
    std::string location = "." + local + "/" + username + ".txt";
    outFile.open(location, std::ios::app); 

    if (outFile.is_open())
    {
        outFile << index;
        outFile.close();
    }
    else
    {
        std::cout << "Die Datei konnte nicht geöffnet werden.\n";
    }
}

void fileHandeling::SAFE_to_File(std::string local, std::string username, TEXTpreset tp)    //speichwert den sender das subject und die message
{
    std::ofstream outFile;
    std::string location = local + "/" + username + ".txt";
    outFile.open(location, std::ios::app); 

    if (outFile.is_open())
    {
        outFile << tp.receiver << "\n";
        outFile << tp.subject << "\n";
        outFile << tp.message << "\n";
        outFile << "[break/$TEXT$/$LOCAL$]\n";
        outFile.close();
        std::cout << "Text wurde hinzugefügt.\n";
    }
    else
    {
        std::cout << "Die Datei konnte nicht geöffnet werden.\n";
    }
}