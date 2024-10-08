#include <iostream>
#include <vector>
#include <getopt.h>
#include <filesystem>
#include <array>
#include <vector>
#include <string.h>
#include <thread>
#include <sstream>

namespace fs = std::filesystem;

void print_usage(std::string path)  // failure event wenn falscher syntax
{
    std::cerr << "Usage: " << path << " [-R] [-i] searchpath filename1 [filename2]\n";
    exit(EXIT_FAILURE);
}

bool testDir(const std::string &directory)  // testet ob ein verzeichnis existiert oder nicht
{
    if (fs::exists(directory) && fs::is_directory(directory))
    {
        std::cout << "Verzeichnis existiert: " << fs::absolute(directory) << "\n";
        return true;
    }
    else
    {
        std::cerr << "Fehler: Verzeichnis konnte nicht geöffnet werden: " << directory << "\n";
        return false;
    }
}

bool fileExists(const std::string &filePath)        // wird benutzt um zu testen ob eine angegeben file existiert
{
    return fs::exists(filePath) && fs::is_regular_file(filePath);
}

bool fileExistsCaseSensitive(const std::string &dirPath, const std::string &fileName)
{
    for (const auto &entry : fs::directory_iterator(dirPath))       // wie die function schon sagt wird getestet ob der filename exakt stimmt
    {
        if (entry.is_regular_file())
        {
            if (entry.path().filename().string() == fileName)
            {
                return true;
            }
        }
    }
    return false;
}

void threadTestFile(int ExtSize, std::array<std::string, 10> commonExt, std::string filename, std::string searchpath, bool sensitive)
{
    bool found = false;                 // wenn die gesuchte file gefunden wurde wird das gesetzt zum returnen 
    bool removedExt = false;            // bool die testet ob eine extension von einer file entfernt wurde. wenn ja dann wird sie true
    bool hasExt = false;                // wenn eine file ohne extension übergeben wurde ist das false um dem programm zu sagen das sie keine extension hat

    int hasNotExtCount = 0;             // wenn eine File mit .XXX z.B. endet wird die extension gesucht wenn sie nicht gefunden wurde wird der zähler benutzt um zu testen welche extension schon getestet wurder
    int NoExtCounter = 0;               // das selbe prinzip wie oben nur ohne extension und es wird gezählt wie oft gesucht wurde ohne eine passende extension zu finden

    std::ostringstream endString;       // wir für die Formatierung benutzt damit nicht wenn die threads joinen die ausgabe irgendwie aussieht
    std::string absPath = fs::absolute(searchpath).generic_string();    // kurze version vom absoluten path

    if ((sensitive) ? fileExistsCaseSensitive(absPath, filename) : fileExists(absPath + filename))      // wenn die file einfach gefunden wurde passiert das
    {                                                                                                   // je nachdem ob -i gesetzt wurde oder nicht. ternary operator wird verwendet

            endString << "\033[32mFile: \"" << filename << "\" Exists in Directory: " << absPath + filename << "\033[0m\n";                                             // sollte eigentlich für den endstring der am ende ausgegeb wird benutzt werden jedoch kam es zu problemen deshalb der std::cout in der nächsten zeile
            std::cout << "\033[32mFile: \"" << filename << "\" Exists in Directory: " << fs::absolute(searchpath).generic_string() + filename << "\033[0m\n";
        
        found = true;
    }
    else            // falls die angegebene datei nicht gleich gefunden wurde versucht das programm andere files zu finden die dem angegebenen File ähnlich sind
    {

        for (int i = 0; i < end(commonExt) - begin(commonExt); i++)     // diese for schleife wird benutzt um zu testen ob die file keine extension hat
        {
            if (filename.find(commonExt[i]) == std::string::npos)           // wenn sie eine extension hat wird hasExt gesetzt
            {
                hasExt = true;  
                hasNotExtCount++;
            }
        }

        if (hasNotExtCount >= ExtSize)          // gibt aus ob die datei ohne extension gefunden wurde. z.B: wenn argument "test" ist und eine file "test" existiert ohne extension
        {
            endString << "\033[31mFile: \"" << filename << "\" without Extension not found\033[0m\n";
            // std::cout << "\033[31mFile: \"" << filename << "\" without Extension not found\033[0m\n"; //auskommentiert für debug Reasons
        }

        if (hasExt == true)             // diese function testet ob eine file existiert die die selbe extension hat. Dies ist nur in speziellen Fällen notwendig jedoch als intermediat step notwendig da sie extensions löscht um die suche später leichter zu machen
        {
            for (int i = 0; i < end(commonExt) - begin(commonExt); i++)
            {
                if (filename.find(commonExt[i]) != std::string::npos)           // es werden alle extension durchgegangen bis eine passende gefunden wurde.
                {
                    endString << "\033[31mFile: \"" << filename << "\" with \"" << commonExt[i] << "\" not found\033[0m\n";

                    // std::cout << "\033[31mFile: \"" << filename << "\" with \"" << commonExt[i] << "\" not found\033[0m\n"; //auskommentiert für debug Reasons
                    
                    filename.erase(filename.find(commonExt[i]), commonExt[i].length());             // wenn die extension nicht gefunden wurde wird die extensio gelöscht und eine andere wurd ausprobiert
                    removedExt = true;                      
                }
            }
        }

        if (((!hasExt || removedExt) && !found) && !sensitive)      // diese funktion wird ausgeführt wenn die datei eine andere extension haben könnte.
        {
            for (int i = 0; i < end(commonExt) - begin(commonExt); i++)
            {
                if ((sensitive) ? fileExistsCaseSensitive(fs::absolute(searchpath).generic_string(), filename + commonExt[i]) : fileExists(fs::absolute(searchpath).generic_string() + filename + commonExt[i])) // testet ob es eine file mit dem selben namen gibt aber einer anderen extension. alle extensions die erlaubt sind werden durch gegangen
                {
                    
                    // der nachfolgende code wird im endstring gespeichert wenn eine andere datei mit dem selben namen aber anderer extension gefunden wurde
                    endString << "  \033[33m-File: with Extension Found!\033[0m\n"
                              << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n";
                    
                    // std::cout << "  \033[33m-File: with Extension Found!\033[0m\n"
                    //           << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n"; //auskommentiert für debug Reasons
                    
                    found = true;
                }
            }
        }

        if (((hasExt && removedExt) && !found) && !sensitive)       // selbe funktion wie die funktion davor nur speziell ausgelegt für files die ohne extension angegeben wurden
        {
            for (int i = 0; i < end(commonExt) - begin(commonExt); i++)
            {
                if ((sensitive) ? fileExistsCaseSensitive(fs::absolute(searchpath).generic_string(), filename + commonExt[i]) : fileExists(fs::absolute(searchpath).generic_string() + filename + commonExt[i]))
                {
                    endString << "  \033[33m-File: \"" << filename << "\" with Extension Found!\033[0m\n"
                              << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n";
                    // std::cout << "  \033[33m-File: with Extension Found!\033[0m\n"
                    //           << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n"; //auskommentiert für debug Reasons
                }
                else
                {
                    NoExtCounter++;
                }
            }

            if (NoExtCounter >= ExtSize)    // sollte überhaupt keine file gefunden werden auch nicht mit anderen extensions wird das ausgegeben
            {
                endString << "  \033[31m-File: \"" << filename << "\" with Extension not Found\033[0m\n";
                // std::cout << "  \033[31m-File: \"" << filename << "\" with Extension not Found\033[0m\n";
            }
        }

        if (((!hasExt || !removedExt) && !found) && !sensitive) // erneut selbe funktion nur für spezielle fälle. trifft fast nicht auf jedoch ist die option vorhanden
        {
            for (int i = 0; i < end(commonExt) - begin(commonExt); i++)
            {
                if ((sensitive) ? fileExistsCaseSensitive(fs::absolute(searchpath).generic_string(), filename + commonExt[i]) : fileExists(fs::absolute(searchpath).generic_string() + filename + commonExt[i]))
                {
                    endString << "  \033[33m-File: \"" << filename << "\" with Extension Found!\033[0m\n"
                              << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n";
                    // std::cout << "  \033[33m-File: \"" << filename << "\" with Extension Found!\033[0m\n"
                    //           << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n"; //auskommentiert für debug Reasons
                }
                else
                {
                    NoExtCounter++;
                }
            }

            if ((NoExtCounter >= ExtSize))
            {
                endString << "  \033[31m-File: \"" << filename << "\" with Extension not Found\033[0m\n";
                // std::cout << "  \033[31m-File: \"" << filename << "\" with Extension not Found\033[0m\n"; //auskommentiert für debug Reasons
            }
        }
        std::string output = endString.str();   // ausgabe aller std::cout auf einmal um formatierung aufrecht zu behalten
        std::cout << output;
    }
}

bool testForFile(int argc, char *argv[], std::string tmpPath, bool sensitive)
{

    std::array<std::string, 10> commonExt = {".txt", ".c", ".cpp", ".cs", ".py", ".csv", ".exe", ".html", ".css", ".js"};       // extension die geprüft werden sollen
    int ExtSize = commonExt.size();         // extension array size
    std::string searchpath = tmpPath;

    std::vector<std::string> filenames;

    for (int i = optind + 1; i < argc; i++)     // alle argumente nach dem searchpath werden gespeichert
    {                                           //
        filenames.push_back(argv[i]);           //
    }                                           //

    std::cout << "Search path: " << searchpath << std::endl;
    if (testDir(searchpath))
    {

        std::vector<std::thread> tVect;         // wird benutzt um die threads dynamisch zu erstellen und sie dynamisch zu joinen

        for (auto &filename : filenames)
        {
            tVect.emplace_back(threadTestFile, ExtSize, commonExt, filename, searchpath, sensitive);    // threads werden erstellt
        }

        for (auto &t : tVect)
        {
            t.join();                   // threads joinen vom vector tVect
        }

        std::cout << std::endl;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int options;                                // optionen für getopt
    int argsCount = 1;                          // diese Variable zählt alle "escape" argumente z.B: -R oder -i
    bool R_flag = false, i_flag = false;        // flags für -R, -i
    int i = 10;                                 // maximale anzahl an File Paths die mit -R getestet werden sollen

    std::string tmpPath = argv[optind];         // path vom user eingegeben
    std::string testPath = "";                  // Path in dem die File gesucht wird (verändert sich bei -R jede wiederholung)
    std::string tmpTestPath = "";               // wenn tmpTestPath und testPath das selbe ist dann im allerletzten directory angekommen und stoppt -R suche

    while ((options = getopt(argc, argv, "Ri")) != -1)  // options bekommt escape argumente
    {
        switch (options)
        {
        case 'R':
            R_flag = true;
            std::cout << "Option -R provided" << std::endl;
            break;
        case 'i':
            i_flag = true;
            std::cout << "Option -i provided" << std::endl;
            break;
        case '?':
            std::cout << "Enter a Valid Expression!" << std::endl;
            print_usage(argv[0]);
            break;
        default:
            print_usage(argv[0]);
            break;
        }
    }

    if (R_flag)                 // erhöht argsCount wenn -R oder -i ausgewählt wurde
    {                           //
        argsCount++;            //
    }                           //
    if (i_flag)                 //
    {                           //
        argsCount++;            //
    }                           //

    tmpPath = argv[argsCount];      // resetet tmpPath wegen -R oder -i (sonst würde -R oder -i als path weitergegeben werden)

    if (R_flag)                                                     // aendert recursive or non recursive 
    {
        testPath = fs::absolute(tmpPath).generic_string();          // speichert ansoluten path ab für tmpTestPath = testPath abfragung
        if (i_flag)                                         // aendert ob sensitive oder nicht
        {                                                   //
            testForFile(argc, argv, tmpPath, false);        // nicht sensitive
        }                                                   //
        else                                                //
        {                                                   //
            testForFile(argc, argv, tmpPath, true);         // sensitiv (genaue rechtschreibung)
        }                                                   //

        do
        {

            if (testPath != tmpTestPath)                // checkt ob der Path der selbe ist wie vorhin wenn ja wird die schleife gebreakt
            {
                tmpTestPath = testPath;
            }
            else if (testPath == tmpTestPath)
            {
                break;
            }

            tmpPath += "../";

            if (i_flag)                                         // aendert ob sensitive oder nicht
            {                                                   //
                testForFile(argc, argv, tmpPath, false);        // nicht sensitive
            }                                                   //
            else                                                //
            {                                                   //
                testForFile(argc, argv, tmpPath, true);         // sensitiv (genaue rechtschreibung)
            }

            i--;
            testPath = fs::absolute(tmpPath).generic_string();

        } while (i > 1);
    }
    else
    {
        if (i_flag)                                         // aendert ob sensitive oder nicht
        {                                                   //
            testForFile(argc, argv, tmpPath, false);        // nicht sensitive
        }                                                   //
        else                                                //
        {                                                   //
            testForFile(argc, argv, tmpPath, true);         // sensitiv (genaue rechtschreibung)
        }
    }

    if (optind >= argc - 1)             // Fehler ausgabe wenn falsche argumente
    {
        print_usage(argv[0]);
    }

    return 0;
}
