#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <cmath>

#include "headers/PresetStruct.h"
#include "classes/parseClass.h"
#include "classes/fileHandeling.h"
#include "classes/messageClass.h"

std::mutex mtx;

#define _BLOCK_SIZE 1024

recvReturn recvFunctBasic(int client_socket)
{
    recvReturn rr;
    char buffer[1024] = {};
    memset(buffer, 0, sizeof(buffer));
    int err = recv(client_socket, buffer, sizeof(buffer), 0);
    if (err == -1)
    {
        rr.buffer = "ERR";
        rr.err = err;
        return rr;
    }
    buffer[err] = '\0';
    rr.buffer = buffer;
    rr.err = err;
    return rr;
}

std::string recvQuitLogin(int client_socket)
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

INFOpreset SEND_and_CALC_infoString(TEXTpreset tp, int socked_fd)
{
    std::string infoString = "";
    INFOpreset ip;
    ip.textLength = tp.sender.size() + 1 + tp.subject.size() + 1 + tp.message.size() + 1;
    float result = static_cast<float>(ip.textLength) / (_BLOCK_SIZE - 1);
    ip.numPack = std::ceil(result);
    infoString = std::to_string(ip.textLength) + "\n" + std::to_string(ip.numPack) + "\n";
    send(socked_fd, infoString.c_str(), infoString.size(), 0);
    return ip;
}

std::string RCV_and_PARSE_serverResponse(int socked_fd)
{
    char buffer[1024] = {};
    int err = recv(socked_fd, buffer, sizeof(buffer), 0);
    if (err == -1)
    {
        std::cout << "error in RCV_and_PARSE_serverResponse()" << std::endl;
        return "ERR";
    }
    buffer[err] = '\0';
    std::string buff = buffer;
    if (buff == "OK")
    {
        return "OK";
    }
    else
    {
        return "ERR";
    }
    return "ERR";
}

void handle_client(int client_socket, std::string dataLocal)
{
    bool isLoggedIn = false;
    LOGINpreset lp;
    do
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

            lp = parseClass().parseLOGIN(loginString);

            if (lp.username == "stefan")
            {
                std::cout << "Authentic" << std::endl;
                std::string OK = "OK";
                send(client_socket, OK.c_str(), OK.size(), 0);
                isLoggedIn = true;
            }
            else
            {
                std::string ERR = "ERR";
                send(client_socket, ERR.c_str(), ERR.size(), 0);
            }
        }
        else
        {
            std::cout << "CLIENT Comment: " << buffer << std::endl;
        }
    } while (!isLoggedIn);

    while (true)
    {
        recvReturn bufferErr= recvFunctBasic(client_socket);
        if(bufferErr.err == -1){close(client_socket); break;}

        std::istringstream iss(bufferErr.buffer);
        std::string operation;
        iss >> operation;

        if (operation == "SEND")
        {
            std::cout << "SEND" << std::endl;

            bufferErr= recvFunctBasic(client_socket);
            if(bufferErr.err == -1){close(client_socket); break;}

            INFOpreset ip = parseClass().parseINFO(bufferErr.buffer);
            std::string OK = "OK";
            send(client_socket, OK.c_str(), OK.size(), 0);

            std::string sendMSG = messageClass().receiveLongMessage(client_socket, ip);
            TEXTpreset tp = parseClass().parseSEND(sendMSG);

            fileHandeling().SAFE_to_File(dataLocal, lp.username, tp);
        }
        else if (operation == "READ")
        {
            std::cout << "READ" << std::endl;

            bufferErr= recvFunctBasic(client_socket);
            if(bufferErr.err == -1){close(client_socket); break;}

            INFOpreset ip = parseClass().parseINFO(bufferErr.buffer);

            std::string OK = "OK";
            send(client_socket, OK.c_str(), OK.size(), 0);

            bufferErr= recvFunctBasic(client_socket);
            if(bufferErr.err == -1){close(client_socket); break;}

            std::string id = bufferErr.buffer;

            std::vector<TEXTpreset> readFiles = fileHandeling().READ_from_File(dataLocal, lp.username);
            int idNUM = std::stoi(id);

            if (static_cast<int>(readFiles.size()) <= 0)
            {
                std::string ERR = "ERR";
                send(client_socket, ERR.c_str(), ERR.size(), 0);
                continue;
            }
            else
            {
                if(idNUM > static_cast<int>(readFiles.size()) || idNUM < 0){
                    std::string ERR = "ERR";
                    send(client_socket, ERR.c_str(), ERR.size(), 0);
                    continue;
                }
                std::string OK = "OK";
                send(client_socket, OK.c_str(), OK.size(), 0);
            }

            if (RCV_and_PARSE_serverResponse(client_socket) == "ERR")
            {
                std::cout << "ID does not exist or is out of scope" << std::endl;
                continue;
            }

            ip = SEND_and_CALC_infoString(readFiles[idNUM], client_socket); // calc and send info string
            messageClass().sendLongMessage(readFiles[idNUM], ip, client_socket);

            continue;
        }
        else if (operation == "LIST")
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

            if (RCV_and_PARSE_serverResponse(client_socket) == "ERR")
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
                INFOpreset ip = SEND_and_CALC_infoString(entry, client_socket); // calc and send info string
                if (RCV_and_PARSE_serverResponse(client_socket) == "ERR")
                {
                    std::cout << "Error in RCV" << std::endl;
                    continue;
                }
                messageClass().sendLongMessage(entry, ip, client_socket);
                i++;
            }
            continue;
        }
        else if (operation == "DEL")
        {

            bufferErr= recvFunctBasic(client_socket);
            if(bufferErr.err == -1){close(client_socket); break;}


            parseClass().parseINFO(bufferErr.buffer);

            std::string OK = "OK";
            send(client_socket, OK.c_str(), OK.size(), 0);

            bufferErr= recvFunctBasic(client_socket);
            if(bufferErr.err == -1){close(client_socket); break;}

            std::string id = bufferErr.buffer;

            std::vector<TEXTpreset> readFiles = fileHandeling().READ_from_File(dataLocal, lp.username);
            int idNUM = std::stoi(id);
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
    server_addr.sin_addr.s_addr = INADDR_ANY;
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
