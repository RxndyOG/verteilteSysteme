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

void print_usage(std::string path)
{
    std::cerr << "Usage: " << path << " [-R] [-i] searchpath filename1 [filename2]\n";
    exit(EXIT_FAILURE);
}

bool testDir(const std::string &directory)
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

bool fileExists(const std::string &filePath)
{
    return fs::exists(filePath) && fs::is_regular_file(filePath);
}

bool fileExistsCaseSensitive(const std::string &dirPath, const std::string &fileName)
{
    for (const auto &entry : fs::directory_iterator(dirPath))
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
    bool found = false;

    bool removedExt = false;
    bool hasExt = false;
    int hasNotExtCount = 0;
    int NoExtCounter = 0;

    std::ostringstream endString;

    if ((sensitive) ? fileExistsCaseSensitive(fs::absolute(searchpath).generic_string(), filename) : fileExists(fs::absolute(searchpath).generic_string() + filename))
    {
        endString << "\033[32mFile: \"" << filename << "\": " << fs::absolute(searchpath).generic_string() + filename << "\033[0m\n";
        std::cout << "\033[32mFile: \"" << filename << "\": " << fs::absolute(searchpath).generic_string() + filename << "\033[0m\n";
        found = true;
    }
    else
    {

        for (int i = 0; i < end(commonExt) - begin(commonExt); i++)
        {
            if (filename.find(commonExt[i]) == std::string::npos)
            {
                hasExt = true;
                hasNotExtCount++;
            }
        }

        if (hasNotExtCount >= ExtSize)
        {
            endString << "\033[31mFile: \"" << filename << "\" without Extension not found\033[0m\n";
            // std::cout << "\033[31mFile: \"" << filename << "\" without Extension not found\033[0m\n";
        }

        if (hasExt == true)
        {
            for (int i = 0; i < end(commonExt) - begin(commonExt); i++)
            {
                if (filename.find(commonExt[i]) != std::string::npos)
                {
                    endString << "\033[31mFile: \"" << filename << "\" with \"" << commonExt[i] << "\" not found\033[0m\n";
                    // std::cout << "\033[31mFile: \"" << filename << "\" with \"" << commonExt[i] << "\" not found\033[0m\n";
                    filename.erase(filename.find(commonExt[i]), commonExt[i].length());
                    removedExt = true;
                }
            }
        }

        if ((!hasExt || removedExt) && !found)
        {
            for (int i = 0; i < end(commonExt) - begin(commonExt); i++)
            {
                if ((sensitive) ? fileExistsCaseSensitive(fs::absolute(searchpath).generic_string(), filename + commonExt[i]) : fileExists(fs::absolute(searchpath).generic_string() + filename + commonExt[i]))
                {
                    endString << "  \033[33m-File: with Extension Found!\033[0m\n" << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n";
                    // std::cout << "  \033[33m-File: with Extension Found!\033[0m\n"
                    //           << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n";
                    found = true;
                }
            }
        }

        if ((hasExt && removedExt) && !found)
        {
            for (int i = 0; i < end(commonExt) - begin(commonExt); i++)
            {
                if ((sensitive) ? fileExistsCaseSensitive(fs::absolute(searchpath).generic_string(), filename + commonExt[i]) : fileExists(fs::absolute(searchpath).generic_string() + filename + commonExt[i]))
                {
                    endString << "  \033[33m-File: with Extension Found!\033[0m\n" << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n";
                    //std::cout << "  \033[33m-File: with Extension Found!\033[0m\n"
                    //          << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n";
                }
                else
                {
                    NoExtCounter++;
                }
            }

            if (NoExtCounter >= ExtSize)
            {
                endString << "  \033[31m-File: \"" << filename << "\" with Extension not Found\033[0m\n";
                //std::cout << "  \033[31m-File: \"" << filename << "\" with Extension not Found\033[0m\n";
            }
        }

        if ((!hasExt || !removedExt) && !found)
        {
            for (int i = 0; i < end(commonExt) - begin(commonExt); i++)
            {
                if ((sensitive) ? fileExistsCaseSensitive(fs::absolute(searchpath).generic_string(), filename + commonExt[i]) : fileExists(fs::absolute(searchpath).generic_string() + filename + commonExt[i]))
                {
                    endString << "  \033[33m-File: \"" << filename << "\" with Extension Found!\033[0m\n"<< "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n";
                    //std::cout << "  \033[33m-File: \"" << filename << "\" with Extension Found!\033[0m\n"
                    //          << "      \033[33m-Do you mean \"" << filename + commonExt[i] << "\" ? In Directory: " << fs::absolute(searchpath).generic_string() + filename + commonExt[i] << "\033[0m\n\n";
                }
                else
                {
                    NoExtCounter++;
                }
            }

            if (NoExtCounter >= ExtSize)
            {
                endString << "  \033[31m-File: \"" << filename << "\" with Extension not Found\033[0m\n";
                //std::cout << "  \033[31m-File: \"" << filename << "\" with Extension not Found\033[0m\n";
            }
        }
        std::string output = endString.str();
        std::cout << output;
    }
}

bool testForFile(int argc, char *argv[], std::string tmpPath, bool sensitive)
{

    std::array<std::string, 10> commonExt = {".txt", ".c", ".cpp", ".cs", ".py", ".csv", ".exe", ".html", ".css", ".js"};
    int ExtSize = commonExt.size();
    std::string searchpath = tmpPath;

    std::vector<std::string> filenames;

    for (int i = optind + 1; i < argc; i++)
    {
        filenames.push_back(argv[i]);
    }

    std::cout << "Search path: " << searchpath << std::endl;
    if (testDir(searchpath))
    {

        std::vector<std::thread> tVect;

        for (auto &filename : filenames)
        {

            tVect.emplace_back(threadTestFile, ExtSize, commonExt, filename, searchpath, sensitive);
        }

        for (auto &t : tVect)
        {
            t.join();
        }

        std::cout << std::endl;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int options;
    int argsCount = 1;
    bool R_flag = false, i_flag = false;
    int i = 10;
    std::string tmpPath = argv[optind];
    std::string testPath = "";
    std::string tmpTestPath = "";

    while ((options = getopt(argc, argv, "Ri")) != -1)
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

    if (R_flag)
    {
        argsCount++;
    }
    if (i_flag)
    {
        argsCount++;
    }

    tmpPath = argv[argsCount];
    if (i_flag)
    {
        testForFile(argc, argv, tmpPath, false);
    }
    else
    {
        testForFile(argc, argv, tmpPath, true);
    }
    if (R_flag)
    {
        testPath = fs::absolute(tmpPath).generic_string();
        do
        {
            if (testPath != tmpTestPath)
            {
                tmpTestPath = testPath;
            }
            else if (testPath == tmpTestPath)
            {
                break;
            }
            tmpPath += "../";
            if (i_flag)
            {
                testForFile(argc, argv, tmpPath, false);
            }
            else
            {
                testForFile(argc, argv, tmpPath, true);
            }
            i--;
            testPath = fs::absolute(tmpPath).generic_string();
        } while (i > 1);
    }

    if (optind >= argc - 1)
    {
        print_usage(argv[0]);
    }

    return 0;
}
