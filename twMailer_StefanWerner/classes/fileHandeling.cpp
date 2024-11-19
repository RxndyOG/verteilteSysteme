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

void fileHandeling::clearFile(std::string local, std::string username) {
    std::string filename = local + "/" + username + ".txt";
    std::ofstream file(filename, std::ios::out); // Öffnet die Datei im Schreibmodus und überschreibt den Inhalt
    if (file.is_open()) {
        std::cout << "Die Datei \"" << filename << "\" wurde erfolgreich geleert.\n";
        file.close();
    } else {
        std::cerr << "Fehler beim Öffnen der Datei \"" << filename << "\" zum Leeren.\n";
    }
}

std::vector<TEXTpreset> fileHandeling::READ_from_File(std::string local, std::string username)
{
    std::string location = local + "/" + username + ".txt";
    std::ifstream inFile(location);

    std::vector<TEXTpreset> fileREAD;

    if (inFile.is_open())
    {
        std::string line;
        int i = 0;  // Zähler, der bei jedem [break] zurückgesetzt wird
        TEXTpreset tp;

        while (getline(inFile, line))
        {
            // Entferne möglichen Wagenrücklauf (\r) von Windows-Zeilenumbrüchen
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

            // Ignoriere Leerzeilen
            if (line.empty())
            {
                continue;
            }

            // Trennen der Nachrichten, wenn der Trennmarker erreicht wird
            if (line == "[break/$TEXT$/$LOCAL$]")
            {
                // Füge die Nachricht zur Liste hinzu, nur wenn sie nicht leer ist
                if (!tp.message.empty())
                {
                    fileREAD.push_back(tp);
                }
                tp = TEXTpreset();  // Zurücksetzen für die nächste Nachricht
                i = 0;              // Zähler zurücksetzen
                continue;
            }
            else if (i == 0)
            {
                // Die erste Zeile ist der Sender
                tp.sender = line;
            }
            else if (i == 1)
            {
                // Die zweite Zeile ist der Betreff (Subject)
                tp.subject = line;
            }
            else if (i >= 2)
            {
                // Alle weiteren Zeilen sind Teil der Nachricht (Message)
                tp.message.append(line + "\n"); // An die Nachricht anhängen
            }

            i++; // Zähler erhöhen
        }

        // Falls am Ende keine Trennung mehr kam, aber eine Nachricht existiert
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

int fileHandeling::readLoginFailures(std::string local, std::string username){
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

void fileHandeling::SAFE_to_FileLOGIN(std::string local, std::string username, int index)
{
    std::ofstream outFile;
    std::string location = "." + local + "/" + username + ".txt";
    outFile.open(location, std::ios::app); // Öffnen im Append-Modus

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

void fileHandeling::SAFE_to_File(std::string local, std::string username, TEXTpreset tp)
{
    std::ofstream outFile;
    std::string location = local + "/" + username + ".txt";
    outFile.open(location, std::ios::app); // Öffnen im Append-Modus

    if (outFile.is_open())
    {
        outFile << tp.sender << "\n";
        outFile << tp.subject << "\n";
        outFile << tp.message;
        outFile << "[break/$TEXT$/$LOCAL$]\n";
        outFile.close();
        std::cout << "Text wurde hinzugefügt.\n";
    }
    else
    {
        std::cout << "Die Datei konnte nicht geöffnet werden.\n";
    }
}