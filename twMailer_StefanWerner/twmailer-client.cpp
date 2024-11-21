#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <future>
#include <thread>
#include <csignal>

#define _BLOCK_SIZE 1024

#include "headers/PresetStruct.h"         //has all structs and maybe enum not sure yet
#include "classes/parseClass.h"           //has all parse methods
#include "classes/messageClass.h"         //has all message functions (sending to server and recv from server messages)
#include "classes/userInputClass.h"       //has all user input functions ( getline() etc...)
#include "classes/basicSocketFunctions.h" //has all the recv() and send(), and parses from server or client

bool isTimedOut = false;

int cSocket;

void handleSigint(int sig) {
    (void) sig; 
    std::string quit = "QUIT";
    send(cSocket, quit.c_str(), quit.size(), 0);
    std::cout << "Closed connection" << std::endl; 
    exit(1);
}

void timerFunction(int seconds) {
    for (int i = seconds; i > 0; --i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(i%10 == 0){std::cout << "Time out remain: " << i << std::endl;}
    }
    isTimedOut = false;
    std::cout << "You are no longer Timed Out" << std::endl;
}

void client_loop(int socket_fd) // client app
{
    
    char buffer[1024] = {};
    std::string input = "";

    std::string username = "";
    std::string pwd = "";

    LOGINpreset lp;
    recvReturn bufferErr;

    std::signal(SIGINT, handleSigint);

    do // when the user isnt loggen in this is shown
    {

        std::cout << "\n ------- OPEN TERMINAL -------" << std::endl;
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

            if (isTimedOut == true)
            {
                std::cout << "You are timed out" << std::endl;
                input = "";
                continue;
            }

            send(socket_fd, input.c_str(), input.size(), 0);
            lp = userInputClass().LOGINinput();
            std::string LOGINstring = lp.username + "\n" + lp.pwd + "\n";
            send(socket_fd, LOGINstring.c_str(), LOGINstring.size(), 0);

            std::string LOGINerr = basicSocketFunctions().RCV_and_PARSE_serverResponse(socket_fd);
            if (LOGINerr == "ERR")
            {
                input = "";
                std::cout << "You are not Autherized" << std::endl;
                isTimedOut = false;
            }
            if (LOGINerr == "TIME")
            {
                input = "";
                std::cout << "You are timed out for 1 minute!" << std::endl;

                isTimedOut = true;
                auto timerFuture = std::async(std::launch::async, timerFunction, 60);
                continue;
            }
            else
            {
                isTimedOut = false;
            }
        }
    } while (input != "LOGIN");

    while (true) // user is loggen in
    {
        TEXTpreset tp;
        std::string input = "";
        std::cout << "\n ------- OPEN TERMINAL -------" << std::endl;
        std::cout << " [SEND] [READ] [LIST] [DEL] [QUIT]" << std::endl;
        std::getline(std::cin, input);
        if (input.substr(0, 4) == "SEND") // how send works here is pretty simple sends the server the SEND protocoll name, //send the info string // recv the OK or ERR from server // send the message
        {                                 // SEND -> SEND infoString sent -> OK or ERR from server -> sending entire message
            input = "SEND";
            send(socket_fd, input.c_str(), input.size(), 0);

            tp = userInputClass().SENDInput(lp);                                            // ask user for message. tp.sender is sender, tp.subject = subject of message, tp.message is message
            INFOpreset ip = basicSocketFunctions().SEND_and_CALC_infoString(tp, socket_fd); // "calulates" and sends the info string

            if (basicSocketFunctions().RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Error in SERVER response in SEND info string problem" << std::endl;
                continue;
            }

            messageClass().sendLongMessage(tp, ip, socket_fd);

            continue;
        }
        if (input.substr(0, 4) == "READ")
        { // READ -> READ infoString -> OK or ERR from server -> sent id -> OK or ERR from server -> sent OK to server -> OK or ERR from server -> recv long message from server
            input = "READ";
            send(socket_fd, input.c_str(), input.size(), 0);

            std::string id = userInputClass().READInput(lp);
            std::string info = id + "\n";
            int infoSize = info.size();
            std::string infoString = std::to_string(infoSize) + "\n" + std::to_string(0) + "\n";
            send(socket_fd, infoString.c_str(), infoString.size(), 0);

            if (basicSocketFunctions().RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Error in RCV" << std::endl;
                continue;
            }

            send(socket_fd, id.c_str(), id.size(), 0);

            if (basicSocketFunctions().RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "ID does not exist or is out of scope" << std::endl;
                continue;
            }

            std::string OK = "OK";
            send(socket_fd, OK.c_str(), OK.size(), 0);

            bufferErr = basicSocketFunctions().recvFunctBasic(socket_fd);
            if (bufferErr.err == -1)
            {
                std::cout << "Error in SERVER response in READ" << std::endl;
                close(socket_fd);
                break;
            }

            INFOpreset ip = parseClass().parseINFO(bufferErr.buffer);
            std::string sendMSG = messageClass().receiveLongMessage(socket_fd, ip);

            TEXTpreset tp = parseClass().parseSEND(sendMSG);

            std::cout << "Receiver: " << tp.receiver << std::endl;
            std::cout << "Subject: " << tp.subject << std::endl;
            std::cout << "Message: " << tp.message << std::endl;

            continue;
        }
        if (input.substr(0, 4) == "LIST")
        { // LIST -> OK or ERR from server -> OK sent to server -> OK or ERR from server -> OK or ERR from server -> sent OK to server -> recv long message
            input = "LIST";
            send(socket_fd, input.c_str(), input.size(), 0);

            // Empfang der ersten Antwort (OK oder ERR)
            if (basicSocketFunctions().RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Not enough Elements in LIST! 0 Entries" << std::endl;
                continue;
            }

            std::string OK = "OK";
            send(socket_fd, OK.c_str(), OK.size(), 0);

            bufferErr = basicSocketFunctions().recvFunctBasic(socket_fd);
            if (bufferErr.err == -1)
            {
                std::cerr << "Error or connection closed while receiving entry count." << std::endl;
                close(socket_fd);
                break;
            }

            std::string buff(bufferErr.buffer);

            int entries = 0;
            try
            {
                entries = std::stoi(buff);
                if (entries == 0)
                {
                    std::cout << "No elements to List" << std::endl;
                }
            }
            catch (std::exception &e)
            {
                std::cerr << "Invalid entry count received: " << buff << std::endl;
                continue;
            }

            std::cout << "You have " << entries << " entries." << std::endl;

            int i = 0;
            for (int n = 0; n < entries; ++n)
            {

                bufferErr = basicSocketFunctions().recvFunctBasic(socket_fd);
                if (bufferErr.err == -1)
                {
                    std::cerr << "Error or connection closed while receiving INFO string." << std::endl;
                    close(socket_fd);
                    break;
                }

                std::string infoString(bufferErr.buffer);

                INFOpreset ip = parseClass().parseINFO(infoString);
                std::string OK = "OK";
                send(socket_fd, OK.c_str(), OK.size(), 0);

                std::string receivedMsg = messageClass().receiveLongMessage(socket_fd, ip);
                TEXTpreset tp = parseClass().parseSEND(receivedMsg);
                std::cout << " -------------------- " << std::endl;
                std::cout << "ID: " << i << std::endl;
                std::cout << "Receiver: " << tp.receiver << std::endl;
                std::cout << "subject: " << tp.subject << std::endl;
                i++;
            }
            continue;
        }
        if (input.substr(0, 4) == "DEL")
        { // sent DEL -> send DEL infoString -> OK or ERR from server -> sent ID -> OK or ERR from server
            input = "DEL";
            send(socket_fd, input.c_str(), input.size(), 0);

            std::string id = userInputClass().READInput(lp);
            std::string info = id + "\n";
            int infoSize = info.size();
            std::string infoString = std::to_string(infoSize) + "\n" + std::to_string(0) + "\n";
            send(socket_fd, infoString.c_str(), infoString.size(), 0);
            if (basicSocketFunctions().RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Not enough Elements in LIST! 0 Entries" << std::endl;
                continue;
            }

            send(socket_fd, id.c_str(), id.size(), 0);

            if (basicSocketFunctions().RCV_and_PARSE_serverResponse(socket_fd) == "ERR")
            {
                std::cout << "Error in DEL" << std::endl;
                std::cout << "Index out of range or no file existing" << std::endl;
                continue;
            }else{
                std::cout << "Message deleted" << std::endl;
            }
            continue;
        }
        if (input.substr(0, 4) == "QUIT")
        { // QUIT -> client stoped -> server closes thread
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

    cSocket = socket_fd;

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
