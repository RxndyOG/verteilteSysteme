#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <future>

#include "headers/PresetStruct.h"           //has all structs and maybe enum not sure yet
#include "classes/parseClass.h"             //has all parse methods
#include "classes/fileHandeling.h"          //has all the file handeling functions (safe to file, read from file, clear file etc...)
#include "classes/messageClass.h"           //has all message functions (sending to server and recv from server messages)
#include "classes/ldapClass.h"              //has all the ldap functions (momentan eine)
#include "classes/basicSocketFunctions.h"   //has all the recv() and send(), and parses from server or client

#define _BLOCK_SIZE 1024

std::string recvQuitLogin(int client_socket)    // gets the LOGIn and QUIT recv when user isnt logged in
{
    char buffer[1024] = {};
    int err = recv(client_socket, buffer, sizeof(buffer), 0);
    if (err == -1)
    {
        std::cout << "Error in QUIT or LOGIN" << std::endl;
        return "ERR";
    }
    buffer[err] = '\0';
    return buffer;
}



void handle_client(int client_socket, std::string dataLocal)    // client thread
{
    bool isLoggedIn = false;
    LOGINpreset lp;
    int index = 0;
    bool isTimedOut = false;

    auto timerFunction = [&](int seconds) {
        std::string falseInputLocal;

        std::this_thread::sleep_for(std::chrono::seconds(seconds));

        falseInputLocal = "." + dataLocal;
        fileHandeling().clearFile(falseInputLocal, lp.username);
        isTimedOut = false;
        std::cout << "Timeout für Client beendet.\n";
    };

    do  // whilw user isnt logged in
    {
        std::string buffer = recvQuitLogin(client_socket);
        if (buffer == "ERR")
        {
            return;
        }
        if (buffer == "QUIT")
        {
            std::cout << "Client closed Connection" << std::endl;
            close(client_socket);
            return;
        }
        if (buffer == "LOGIN")
        {
            std::string loginString = recvQuitLogin(client_socket);
            if (loginString == "ERR")
            {
                return;
            }
            if(loginString == "QUIT"){
                std::cout << "Client closed Connection" << std::endl;
                return;
            }

            lp = parseClass().parseLOGIN(loginString);

            if (fileHandeling().readLoginFailures(dataLocal, lp.username) > 1)
            {
                std::cout << "To many Failed attemps!" << std::endl;
                std::string TIME = "TIME";
                send(client_socket, TIME.c_str(), TIME.size(), 0);

                isTimedOut = true;
                std::thread(timerFunction, 60).detach();
                
                continue;
            }

            if ((ldapClass().connectToLDAP(lp.username, lp.pwd) == 0) || lp.username == "stefan") // da ich alleine bin muss ich stefan als 2 user benutzen anstatt ein fh account
            {
                std::cout << "Authentic" << std::endl;
                std::string OK = "OK";
                send(client_socket, OK.c_str(), OK.size(), 0);
                isLoggedIn = true;
                std::string falseInputLocal;
                falseInputLocal = "." + dataLocal;
                fileHandeling().clearFile(falseInputLocal, lp.username);
            }
            else
            {

                std::string falseInputLocal;
                falseInputLocal = "." + dataLocal;
                fileHandeling().clearFile(falseInputLocal, lp.username);
                index++;
                fileHandeling().SAFE_to_FileLOGIN(dataLocal, lp.username, index);
                std::string ERR = "ERR";
                send(client_socket, ERR.c_str(), ERR.size(), 0);
            }
        }
        else
        {
            std::cout << "CLIENT Comment: " << buffer << std::endl;
        }
    } while (!isLoggedIn);

    while (true)      // user is logged in
    {
        recvReturn bufferErr = basicSocketFunctions().recvFunctBasic(client_socket);
        if (bufferErr.err == -1)
        {
            close(client_socket);
            break;
        }

        std::istringstream iss(bufferErr.buffer);
        std::string operation;
        iss >> operation;

        if (operation == "SEND")    // recv SEND command from user -> recv info string -> send OK or ERR -> recv long message
        {
            std::cout << "SEND" << std::endl;

            bufferErr = basicSocketFunctions().recvFunctBasic(client_socket);
            if (bufferErr.err == -1)
            {
                std::cout << "Error in client info string in SEND" << std::endl;
                close(client_socket);
                break;
            }
            if(bufferErr.buffer == "QUIT"){
                std::cout << "Client closed Connection" << std::endl;
                return;
            }

            INFOpreset ip = parseClass().parseINFO(bufferErr.buffer);
            std::string OK = "OK";
            send(client_socket, OK.c_str(), OK.size(), 0);

            std::string sendMSG = messageClass().receiveLongMessage(client_socket, ip);
            TEXTpreset tp = parseClass().parseSEND(sendMSG);

            fileHandeling().SAFE_to_File(dataLocal, tp.receiver, tp);
        }
        else if (operation == "READ")       // recv READ from client -> recv info string from user -> send OK or ERR -> recv ID for READ from client -> 
        {                                   // read files -> send OK or ERR if existing -> recv OK or ERR from client -> send READ file
            std::cout << "READ" << std::endl;

            bufferErr = basicSocketFunctions().recvFunctBasic(client_socket);
            if (bufferErr.err == -1)
            {
                std::cout << "Error in sent info string from client" << std::endl;
                close(client_socket);
                break;
            }
            if(bufferErr.buffer == "QUIT"){
                std::cout << "Client closed Connection" << std::endl;
                return;
            }

            INFOpreset ip = parseClass().parseINFO(bufferErr.buffer);

            std::string OK = "OK";
            send(client_socket, OK.c_str(), OK.size(), 0);

            bufferErr = basicSocketFunctions().recvFunctBasic(client_socket);
            if (bufferErr.err == -1)
            {
                std::cout << "Error in client ID in READ" << std::endl;
                close(client_socket);
                break;
            }
            if(bufferErr.buffer == "QUIT"){
                std::cout << "Client closed Connection" << std::endl;
                return;
            }

            std::string id = bufferErr.buffer;
            int idNUM;

            std::vector<TEXTpreset> readFiles = fileHandeling().READ_from_File(dataLocal, lp.username);
            try
            {
                idNUM = std::stoi(id);
            }
            catch (...)
            {
                std::cerr << "ID is not correct" << std::endl;
                std::string ERR = "ERR";
                send(client_socket, ERR.c_str(), ERR.size(), 0);
                continue;
            }

            if (idNUM >= static_cast<int>(readFiles.size()))
            {
                std::string ERR = "ERR";
                send(client_socket, ERR.c_str(), ERR.size(), 0);
                continue;
            }

            if (static_cast<int>(readFiles.size()) <= 0)
            {
                std::string ERR = "ERR";
                send(client_socket, ERR.c_str(), ERR.size(), 0);
                continue;
            }
            else
            {
                if (idNUM > static_cast<int>(readFiles.size()) || idNUM < 0)
                {
                    std::string ERR = "ERR";
                    send(client_socket, ERR.c_str(), ERR.size(), 0);
                    continue;
                }
                std::string OK = "OK";
                send(client_socket, OK.c_str(), OK.size(), 0);
            }

            if (basicSocketFunctions().RCV_and_PARSE_serverResponse(client_socket) == "ERR")
            {
                std::cout << "ID does not exist or is out of scope" << std::endl;
                continue;
            }

            ip = basicSocketFunctions().SEND_and_CALC_infoString(readFiles[idNUM], client_socket); // calc and send info string
            messageClass().sendLongMessage(readFiles[idNUM], ip, client_socket);

            continue;
        }
        else if (operation == "LIST")   // recv LIST from client -> read files -> send OK or ERR when files not existing -> send amount of files -> recv OK or ERR -> send files
        {
            std::cout << "LIST" << std::endl;

            std::vector<TEXTpreset> readFiles = fileHandeling().READ_from_File(dataLocal, lp.username);

            if (static_cast<int>(readFiles.size()) < 0)
            {
                std::string ERR = "ERR";
                send(client_socket, ERR.c_str(), ERR.size(), 0);
                continue;
            }
            else
            {
                std::string OK = "OK";
                send(client_socket, OK.c_str(), OK.size(), 0);
            }

            if (basicSocketFunctions().RCV_and_PARSE_serverResponse(client_socket) == "ERR")
            {
                std::cout << "Not enough Elements in LIST! 0 Entries" << std::endl;
                continue;
            }

            std::string entryNum = std::to_string(readFiles.size());
            send(client_socket, entryNum.c_str(), entryNum.size(), 0);

            int i = 0;
            for (auto &entry : readFiles)
            {
                entry.message = entry.message.substr(0, 10);
                INFOpreset ip = basicSocketFunctions().SEND_and_CALC_infoString(entry, client_socket); // calc and send info string
                if (basicSocketFunctions().RCV_and_PARSE_serverResponse(client_socket) == "ERR")
                {
                    std::cout << "Error in client Response in LIST" << std::endl;
                    continue;
                }
                messageClass().sendLongMessage(entry, ip, client_socket);
                i++;
            }
            continue;
        }
        else if (operation == "DEL")            // recv DEL from client -> send OK to client -> recv ID from client -> send OK or ERR to client -> read all files -> delete wanted ID -> send OK to client if deleted or ERR if not
        {

            bufferErr = basicSocketFunctions().recvFunctBasic(client_socket);
            if (bufferErr.err == -1)
            {
                close(client_socket);
                break;
            }
            if(bufferErr.buffer == "QUIT"){
                std::cout << "Client closed Connection" << std::endl;
                return;
            }

            parseClass().parseINFO(bufferErr.buffer);

            std::string OK = "OK";
            send(client_socket, OK.c_str(), OK.size(), 0);

            bufferErr = basicSocketFunctions().recvFunctBasic(client_socket);
            if (bufferErr.err == -1)
            {
                close(client_socket);
                break;
            }
            if(bufferErr.buffer == "QUIT"){
                std::cout << "Client closed Connection" << std::endl;
                return;
            }

            std::string id = bufferErr.buffer;
            int idNUM;
            std::vector<TEXTpreset> readFiles = fileHandeling().READ_from_File(dataLocal, lp.username);
            try
            {
                idNUM = std::stoi(id);
            }
            catch (...)
            {
                std::cerr << "ID is not correct" << std::endl;
                std::string ERR = "ERR";
                send(client_socket, ERR.c_str(), ERR.size(), 0);
                continue;
            }

            if (idNUM <= static_cast<int>(readFiles.size()) && static_cast<int>(readFiles.size()) != 0 && !(idNUM < 0))
            {
                fileHandeling().clearFile(dataLocal, lp.username);
                readFiles.erase(readFiles.begin() + idNUM);
                for (auto &entry : readFiles)
                {
                    fileHandeling().SAFE_to_File(dataLocal, lp.username, entry);
                }
                if (idNUM > static_cast<int>(readFiles.size()))
                {
                    std::string ERR = "ERR";
                    send(client_socket, ERR.c_str(), ERR.size(), 0);
                    continue;
                }
                else
                {

                    std::string OK = "OK";
                    send(client_socket, OK.c_str(), OK.size(), 0);
                }
            }
            else
            {
                std::string ERR = "ERR";
                send(client_socket, ERR.c_str(), ERR.size(), 0);
                continue;
            }
            continue;
        }
        else if (operation == "QUIT")
        {
            std::cout << "Client closed Connection" << std::endl;
            break;
        }
        else
        {
            send(client_socket, "UNKNOWN COMMAND\n", 16, 0);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <PORT> <file Location>\n";
        return 1;
    }

    int port = std::stoi(argv[1]);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1)
    {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_socket, 5) == -1)
    {
        perror("Listen failed");
        return 1;
    }

    std::cout << "Server listening on port " << port << "...\n";

    while (true)
    {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_len);
        if (client_socket == -1)
        {
            perror("Accept failed");
            continue;
        }
        std::string local = argv[2];

        std::thread([client_socket, local]()
                    { handle_client(client_socket, local); })
            .detach();
    }

    close(server_socket);
    return 0;
}
