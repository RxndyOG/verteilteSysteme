#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cmath>

#include <cctype>

struct TextPreset
{
    int packageNUM;
    std::string delim;
    int length;
    int type;
    std::string argument;
    std::string sender;
    std::string subject;
    std::string text;
};

std::string convertToLowercase(const std::string &str)
{
    std::string result = "";
    for (char ch : str)
    {
        result += tolower(ch);
    }
    return result;
}

std::string SENDFunct()
{

    std::string input;
    std::string result = "";

    std::cout << "Enter Sender: ";
    std::cin.ignore();
    getline(std::cin, input);
    result += ":";
    result += input;

    std::cout << "Enter Subject [MAX 80 Char]: ";
    getline(std::cin, input);
    input = input.substr(0, 80);
    result += ":";
    result += input;

    std::cout << "Enter Message: " << std::endl;
    int i = 1;
    do
    {
        std::cout << "Line " << i << ": ";
        getline(std::cin, input);
        result += ":";
        result += input;
        i++;
    } while (input != ".");

    result += ":";
    return result;
}

std::string READFunct()
{

    std::string input;
    std::string result = "";

    std::cout << "Enter Username: ";
    std::cin.ignore();
    getline(std::cin, input);
    result += ":";
    result += input;

    std::cout << "Enter Message Number: ";
    getline(std::cin, input);
    result += ":";
    result += input;
    result += ":";

    return result;
}

void SENDmessageRecv(char buffer[4096])
{

    std::string pch;

    pch = buffer;

    for (std::string::size_type i = 0; i < pch.length(); i++)
    {
        if (pch[i] == ':')
        {
            std::cout << std::endl;
        }
        else
        {
            std::cout << pch[i];
        }
    }
}

void receiveFromServer(int clientSocket, bool stand)
{
    char buffer[1024] = {0}; // Puffer initialisieren
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0)
    {
        if (stand == true)
        {
            SENDmessageRecv(buffer);
        }
        else
        {
            std::cout << "Server: " << buffer << std::endl;
        }
    }
    else if (bytesReceived == 0)
    {
        std::cout << "Connection closed by server." << std::endl;
    }
    else
    {
        std::cerr << "recv() failed" << std::endl;
    }
}

int sendToServer(int clientSocket, const char *clientMessage, int *sendMSGS)
{

    std::string argument = "";

    for (int i = 0; i < 4; i++)
    {
        argument += clientMessage[i];
    }

    if (argument == "SEND") // funktioniert
    {
        argument += SENDFunct();
        send(clientSocket, argument.c_str(), strlen(argument.c_str()), 0);
        (*sendMSGS)++;
        receiveFromServer(clientSocket, false);
        return 2;
    }

    if (argument == "READ") // funktioniert (muss noch mehr getestet werden)
    {

        argument += READFunct();
        send(clientSocket, argument.c_str(), strlen(argument.c_str()), 0);
        receiveFromServer(clientSocket, false);
        receiveFromServer(clientSocket, true);
        return 2;
    }
    else
    {
        send(clientSocket, clientMessage, strlen(clientMessage), 0);

        return 1;
    }

    return 1;
}

void SENDfunct(std::string input, int clientSocket)
{

    struct TextPreset tp;
    tp.type = 1;

    if (input.size() > 4)
    {
        std::cout << "SEND detected: Do you still wish to send the longer sentence [y, n] (You will send it as a Comment)" << std::endl;
        char askLongSend;
        std::cin >> askLongSend;
        std::string TempINFO;
        switch (askLongSend)
        {
        case 'y':
        case 'Y':
            tp.type = -1;
            tp.packageNUM = 0;
            tp.delim = "\\n";
            tp.length = input.size();

            double roundErr;

            if (input.size() > 4096)
            {
                roundErr = input.size() / 4096;
                tp.packageNUM = std::ceil(roundErr);
            }
            else
            {
                tp.packageNUM = 1;
            }

            TempINFO = std::to_string(tp.packageNUM) + '\n' + tp.delim + '\n' + std::to_string(tp.length) + '\n' + std::to_string(tp.type) + '\n';

            if (TempINFO.size() > 4096)
            {
                std::cout << "Error String to long for info" << std::endl;
                return;
            }
            else
            {
                send(clientSocket, TempINFO.c_str(), sizeof(TempINFO.c_str()), 0);
                send(clientSocket, input.c_str(), sizeof(input.c_str()), 0);
            }

            return;
            break;
        case 'n':
        case 'N':
            std::cout << "Not Send" << std::endl;
            break;
        default:
            std::cout << "Wrong input: Not Send" << std::endl;
            break;
        }
    }

    std::cout << "Sender: " << std::endl;
    std::string sender;
    std::getline(std::cin, sender);
    tp.sender = sender;
    std::string subject;
    do
    {
        std::cout << "Subject: " << std::endl;
        std::getline(std::cin, subject);
        if (subject.size() > 80)
        {
            std::cout << "Subject is to long (max 80)" << std::endl;
        }
    } while (subject.size() > 80);
    tp.subject = subject;
    std::cout << "Message: " << std::endl;
    std::vector<std::string> messageVect;
    std::string messageText;
    do
    {
        messageText = "";
        std::getline(std::cin, messageText);
        messageVect.push_back(messageText);
    } while (messageText != ".");
    std::string SENDstring = "";
    SENDstring = SENDstring + "SEND" + '\n' + tp.sender + '\n' + tp.subject + '\n';

    for (std::string &i : messageVect)
    {
        SENDstring = SENDstring + i + '\n';
    }

    std::vector<std::string> SENDsize;
    tp.length = SENDstring.size();

    double roundErr;

    if (SENDstring.size() > 4096)
    {
        roundErr = SENDstring.size() / 4096;
        tp.packageNUM = std::ceil(roundErr);
    }
    else
    {
        tp.packageNUM = 1;
    }
    tp.delim = "\\n";

    std::string infoString = "";
    infoString = std::to_string(tp.packageNUM) + '\n' + tp.delim + '\n' + std::to_string(tp.length) + '\n' + std::to_string(tp.type) + '\n';

    if (infoString.size() > 4096)
    {
        std::cout << "Error String to long for info" << std::endl;
    }
    else
    {
        send(clientSocket, infoString.c_str(), infoString.size(), 0);
        char buffer[4096];
        int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[errRecv] = '\n';
        if(errRecv == -1){
            std::cout << "error happened during recv from server" << std:: endl;
        }
        std::string OK = "OK\n";
        std::string ERR = "ERR\n";
        if(buffer == OK){
            std::cout << "OK" << std::endl;
            send(clientSocket, SENDstring.c_str(), SENDstring.size(), 0);
        }else if(buffer == ERR){
            std::cout << "ERR" << std::endl;
            return;
        }
    }
}

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        printf("error");
    }

    // int sendMSGS = 0;
    std::string ipAddress = argv[1];
    if (convertToLowercase(ipAddress) == "localhost")
    {
        ipAddress = "127.0.0.1";
    }

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Connect to the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2]));                // Port number
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str()); // Server IP

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Error connecting to server" << std::endl;
        close(clientSocket);
        return 2;
    }

    std::cout << "listening on Port: " << argv[2] << std::endl;

    // Receive data from the serve
    receiveFromServer(clientSocket, false);
    // receiveFromServer(clientSocket,false);
    // sendToServer(clientSocket, "Hello from the Client", &sendMSGS);

    std::string input;

    do
    {
        std::cout << "\n --------- Open Terminal ----------" << std::endl;
        std::cout << "[SEND] [READ] [DEL] [QUIT]" << std::endl;

        std::getline(std::cin, input);

        // need to remove before launch
        // test if input bigger than 4 set send to block

        std::cout << input.size() << std::endl;

        if (input.size() > 4)
        {
        }

        if (input.substr(0, 4) == "SEND")
        {
            SENDfunct(input, clientSocket);
        }
        else if (input.substr(0, 4) == "READ")
        {
        }
        else if (input.substr(0, 4) == "LIST")
        {
        }
        else if (input.substr(0, 3) == "DEL")
        {
        }
        else if (input.substr(0, 4) == "QUIT")
        {
            break;
        }
        else
        {
            send(clientSocket, input.c_str(), sizeof(input.c_str()), 0);
        }

        /*
                if (sendToServer(clientSocket, input.c_str(), &sendMSGS) == 0)
                {
                    receiveFromServer(clientSocket, false);
                }
        */
    } while (input != "QUIT");

    std::cout << "quiting in prozess" << std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}