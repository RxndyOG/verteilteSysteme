#include <iostream>
#include <string>
#include <filesystem>
#include <array>
#include <algorithm>
#include <string.h>

#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h> //existiert nur in linux bzw. funktioniert nur in linux

namespace fs = std::filesystem;

int NUM_PROCESSES = 10;
bool iSensitive = true;

void print_usage(std::string path) // zeigt an wie die syntax des codes ist
{
    std::cerr << "Usage: " << path << " [-R] [-i] searchpath filename1 [filename2]\n";
    exit(EXIT_FAILURE);
}

bool fileExists(const std::string &filePath) // wird benutzt um zu testen ob eine angegeben file existiert egal wie sie heiSSt
{
    std::string dirPath = fs::path(filePath).parent_path().string(); // extrahiert den path und den namen
    std::string fileName = fs::path(filePath).filename().string();

    // Umwandlung des Dateinamens in Kleinbuchstaben
    std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower); // transform wird benutzt um strings zu aendern in dem fall mittels ::tolower zu kleinbuchstaben

    try // try catch falls es zu einem error kommt
    {
        if (fs::exists(dirPath) && fs::is_directory(dirPath)) // exists() testet ob das directory existiert und is_directory() ob es ein echtes verzeichnis ist
        {
            for (const auto &entry : fs::directory_iterator(dirPath)) // der directory_iterator() rennt durch alle files durch und schaut ob eines passt
            {
                std::string entryName = entry.path().filename().string();
                std::transform(entryName.begin(), entryName.end(), entryName.begin(), ::tolower); // hier wird die durchgelaufene file auf kleinbuchstaben gesetzt damit nicht alle versionen (z.B.: Test.c, TEST.c etc...) getestet werden muessen
                if (entryName == fileName)
                {
                    return true;
                }
            }
        }
    }
    catch (const fs::filesystem_error &e) // sollte es zu einem problem kommen wird der error gecatched und als std::cerr ausgegeben
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }

    return false;
}

bool fileExistsCaseSensitive(const std::string &dirPath, const std::string &fileName) // testet ob sie existiert
{
    for (const auto &entry : fs::directory_iterator(dirPath)) // wie die function schon sagt wird getestet ob der filename exakt stimmt
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

void findFileInCurrentDir(std::string searchPath, char *argv[], int i, bool R_flag) // sucht die gewünschte file im directory
{
    std::string prevPath; // wird benutzt um zu testen ob der path am ende angekommen ist wenn ja breaked es die search

    do
    {

        if (prevPath == fs::canonical(fs::absolute(searchPath).generic_string() + "/")) // testet ob am ende angekommen
        {
            break;
        }

        
        std::string tmpPathName = fs::absolute(searchPath).generic_string(); // speichert den searchpath als absolute da es einfacher ist damit zu arbeiten
        std::string tmpFileName = argv[optind + i + 1];                      // speichert die gewuenschte file als string ab
        fs::path newTmpPathCanon = fs::canonical(tmpPathName);               // sollte ein ../ im filename sein wird sie hier canonalisiert
        newTmpPathCanon += "/";                                              //
        newTmpPathCanon += tmpFileName;                                      // gibt die file dazu

        std::cout << "Current File Path Search of Procces: " << getpid() << " : " << fs::canonical(fs::absolute(searchPath).generic_string() + "/") << std::endl; // sagt dem user wo gerade getestet wird


        // std::cout << newTmpPathCanon << std::endl;    // test für debug als test ob path existiert

        if ((iSensitive) ? fileExistsCaseSensitive(tmpPathName + "/", tmpFileName) : fileExists(newTmpPathCanon)) // ternary operator für i-flag on genau oder ungenau gesucht werden soll
        {                                                                                                         // der if block sucht mittels file path die file

            std::cout << getpid() << ": " << argv[optind + 1] << ": " << fs::absolute(newTmpPathCanon).generic_string() << std::endl; // gibt den filepath aus
        }
        else
        {
            std::cout << getpid() << ": " << tmpFileName << ": Not Found" <<  std::endl; // gibt eine fehlermneldung aus
        }

        prevPath = fs::canonical(fs::absolute(searchPath).generic_string() + "/"); // resets den prevPath für die erkennung des letzten directories

        searchPath += "../"; // geht ein directory weiter nach unten

    } while (true);
}

int main(int argc, char *argv[])
{
    int options;         // optionen für getopt
    bool R_flag = false; // R flage für recursive search

    while ((options = getopt(argc, argv, "Ri")) != -1) // options bekommt escape argumente
    {
        switch (options)
        {
        case 'R':
            R_flag = true;
            std::cout << "Option -R provided" << std::endl;
            break;
        case 'i':
            iSensitive = false;
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

    NUM_PROCESSES = argc - (optind + 1); // aendert die anzahl an processen je nachdem wie viele argumente
                                         // jeder process hat eine file zum testen

    std::string tmpPath = argv[optind]; // speichert den vom user eingegebenen path

    pid_t parent[NUM_PROCESSES]; // erstellt den parent process

    int i = 0;

    while (i < NUM_PROCESSES)
    {

        pid_t Cpid = fork(); // child Proccess wird erstellt mittels fork

        if (Cpid == -1) // error handeling
        {
            perror("Failed to fork!");
            exit(EXIT_FAILURE);
        }

        if (Cpid == 0) // wenn child erfolgreich erstellt wurde
        {
            findFileInCurrentDir(tmpPath, argv, i, R_flag);
            exit(EXIT_SUCCESS);
        }
        else // wenn fertig geht es eines tiefer also zum naechsten argument
        {
            parent[i] = Cpid;
            i++;
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++) // wartet auf die children
    {
        waitpid(parent[i], nullptr, 0);
    }

    if (optind >= argc - 1) // Fehler ausgabe wenn falsche argumente
    {
        print_usage(argv[0]);
    }

    return 0;
}