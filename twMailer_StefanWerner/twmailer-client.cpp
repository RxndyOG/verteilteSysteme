#include <iostream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>

#define _BLOCK_SIZE 1024   

#include "headers/PresetStruct.h"   //has all structs and maybe enum not sure yet
#include "classes/parseClass.h"     //has all pase methods
#include "classes/messageClass.h"   //has all message functions (sending to server and recv from server)
#include "classes/userInputClass.h"

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

void client_loop(int socket_fd)
{

    char buffer[1024] = {};
    std::string input = "";

    std::string username = "";
    std::string pwd = "";

    LOGINpreset lp;
    recvReturn bufferErr;

    do
    {
        std::cout << " ------- OPEN TERMINAL -------" << std::endl;
        std::cout << " [LOGIN] [QUIT]" << std::endl;
        std::getline(std::cin, input);
        if (input == "QUIT")
        {
            send(socket_fd, input.c_str(), input.size(), 0);
            std::cout << "Quiting in Process" << std::endl;
            close(socket_fd);
            return;
        }
        if (input == "LOGIN")
        {
            send(socket_fd, input.c_str(), input.size(), 0);
            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            std::cout << "Enter PWD: ";
            std::getline(std::cin, pwd);
            std::string LOGINstring = username + "\n" + pwd + "\n";
            send(socket_fd, LOGINstring.c_str(), LOGINstring.size(), 0);
            if (RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                input = "";
                std::cout << "You are not Autherized" << std::endl;
            }
            else
            {
                lp.username = username;
                lp.pwd = pwd;
            }
        }
    } while (input != "LOGIN");

    while (true)
    {
        TEXTpreset tp;
        std::string input = "";
        std::cout << " ------- OPEN TERMINAL -------" << std::endl;
        std::cout << " [SEND] [READ] [LIST] [DEL] [QUIT]" << std::endl;
        std::getline(std::cin, input);
        if (input.substr(0, 4) == "SEND")   // how send works here is pretty simple sends the server the SEND protocoll name, //send the info string // recv the OK or ERR from server // send the message
        {                   // SEND -> SEND infoString sent -> OK or ERR from server -> sending entire message      
            input = "SEND";
            send(socket_fd, input.c_str(), input.size(), 0);

            tp = userInputClass().SENDInput(lp);                     // ask user for message. tp.sender is sender, tp.subject = subject of message, tp.message is message
            INFOpreset ip = SEND_and_CALC_infoString(tp, socket_fd);    // "calulates" and sends the info string

            if (RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Error in RCV" << std::endl;
                continue;
            }

            messageClass().sendLongMessage(tp, ip, socket_fd);

            continue;
        }
        if (input.substr(0, 4) == "READ")       
        {                   // READ -> READ infoString -> OK or ERR from server -> sent id -> OK or ERR from server -> sent OK to server -> OK or ERR from server -> recv long message from server
            input = "READ";
            send(socket_fd, input.c_str(), input.size(), 0);

            std::string id = userInputClass().READInput(lp);
            std::string info = id + "\n";
            int infoSize = info.size();
            std::string infoString = std::to_string(infoSize) + "\n" + std::to_string(0) + "\n";
            send(socket_fd, infoString.c_str(), infoString.size(), 0);

            if (RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Error in RCV" << std::endl;
                continue;
            }

            send(socket_fd, id.c_str(), id.size(), 0);

            if (RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "ID does not exist or is out of scope" << std::endl;
                continue;
            }
            
            std::string OK = "OK";
            send(socket_fd, OK.c_str(), OK.size(), 0);

            bufferErr= recvFunctBasic(socket_fd);
            if(bufferErr.err == -1){close(socket_fd); break;}
            
            INFOpreset ip = parseClass().parseINFO(bufferErr.buffer);
            std::string sendMSG = messageClass().receiveLongMessage(socket_fd, ip);
            TEXTpreset tp = parseClass().parseSEND(sendMSG);

            std::cout << "Sender:  " << tp.sender << std::endl;
            std::cout << "Subject: " << tp.subject << std::endl;
            std::cout << "Message: " << tp.message << std::endl;

            continue;
        }
        if (input.substr(0, 4) == "LIST")
        {               // LIST -> OK or ERR from server -> OK sent to server -> OK or ERR from server -> OK or ERR from server -> sent OK to server -> recv long message
            input = "LIST";
            send(socket_fd, input.c_str(), input.size(), 0);

            // Empfang der ersten Antwort (OK oder ERR)
            if (RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Not enough Elements in LIST! 0 Entries" << std::endl;
                continue;
            }

            std::string OK = "OK";
            send(socket_fd, OK.c_str(), OK.size(), 0);

            bufferErr= recvFunctBasic(socket_fd);
            if(bufferErr.err == -1){std::cerr << "Error or connection closed while receiving entry count." << std::endl; close(socket_fd); break;}

            std::string buff(bufferErr.buffer);

            int entries = 0;
            try
            {
                entries = std::stoi(buff);
            }
            catch (std::exception &e)
            {
                std::cerr << "Invalid entry count received: " << buff << std::endl;
                continue;
            }

            int i = 0;
            // Verarbeitung der Einträge
            for (int n = 0; n < entries; ++n)
            {

                bufferErr= recvFunctBasic(socket_fd);
                if(bufferErr.err == -1){std::cerr << "Error or connection closed while receiving INFO string." << std::endl; close(socket_fd); break;}
    
                std::string infoString(bufferErr.buffer);

                INFOpreset ip = parseClass().parseINFO(infoString);
                std::string OK = "OK";
                send(socket_fd, OK.c_str(), OK.size(), 0);

                std::string receivedMsg = messageClass().receiveLongMessage(socket_fd, ip);
                TEXTpreset tp = parseClass().parseSEND(receivedMsg);
                std::cout << "ID: " << i << std::endl;
                std::cout << "sender: " << tp.sender << std::endl;
                std::cout << "subject: " << tp.subject << std::endl;
                i++;
            }
            continue;
        }
        if (input.substr(0,4) == "DEL")
        {               // sent DEL -> send DEL infoString -> OK or ERR from server -> sent ID -> OK or ERR from server
            input = "DEL";
            send(socket_fd, input.c_str(), input.size(), 0);

            std::string id = userInputClass().READInput(lp);
            std::string info = id + "\n";
            int infoSize = info.size();
            std::string infoString = std::to_string(infoSize) + "\n" + std::to_string(0) + "\n";
            send(socket_fd, infoString.c_str(), infoString.size(), 0);
            if (RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Not enough Elements in LIST! 0 Entries" << std::endl;
                continue;
            }

            send(socket_fd, id.c_str(), id.size(), 0);

            if (RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Error in DEL" << std::endl;
                std::cout << "Index out of range or no file existing" << std::endl;
                continue;
            }
            continue;
        }
        if (input.substr(0,4) == "QUIT")
        {               // QUIT -> client stoped -> server closes thread
            input = "QUIT";
            send(socket_fd, input.c_str(), input.size(), 0);
            std::cout << "Quiting in Process" << std::endl;
            close(socket_fd);
            break;
        }

        std::cout << buffer;
    }

    close(socket_fd);
}

std::string convertToLowercase(const std::string &str)
{
    std::string result = "";
    for (char ch : str)
    {
        result += tolower(ch);
    }
    return result;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <SERVER_IP> <PORT>\n";
        return 1;
    }

    std::string ipAddress = argv[1];
    if (convertToLowercase(ipAddress) == "localhost")
    {
        ipAddress = "127.0.0.1";
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2])); // Port number
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    if (connect(socket_fd, (sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Connection failed");
        return 1;
    }

    std::cout << "Connected to server\n";
    client_loop(socket_fd);

    return 0;
}
