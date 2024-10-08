#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

#include <cctype>
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

    for(std::string::size_type i = 0; i < pch.length(); i++){
        if(pch[i] == ':'){
            std::cout << std::endl;
        }else{
            std::cout << pch[i]; 
        }
    }
}

void receiveFromServer(int clientSocket, bool stand)
{
    char buffer[1024] = {0}; // Puffer initialisieren
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {
        if (stand == true) {
            SENDmessageRecv(buffer);
        } else {
            std::cout << "Server: " << buffer << std::endl;
        }
    } else if (bytesReceived == 0) {
        std::cout << "Connection closed by server." << std::endl;
    } else {
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

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        printf("error");
    }

    int sendMSGS = 0;
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

    std::cout << "listening on Port" << argv[2] << std::endl;

    // Receive data from the serve
    receiveFromServer(clientSocket,false);
    receiveFromServer(clientSocket,false);
    sendToServer(clientSocket, "Hello from the Client", &sendMSGS);

    std::string input;

    do
    {
        std::cout << "\n --------- Open Terminal ----------" << std::endl;
        std::cout << "[SEND] [READ] [DEL] [QUIT]" << std::endl;

        std::cin >> input;
        if (sendToServer(clientSocket, input.c_str(), &sendMSGS) == 0)
        {
            receiveFromServer(clientSocket, false);
        }

    } while (input != "QUIT");

    std::cout << "quiting in prozess" << std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}