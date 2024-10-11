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

struct TextPreset parseInfoString(struct TextPreset tp, char buffer[1024])
{

    std::string messageRecv = buffer;
    std::vector<std::string> parseMess;
    std::string tempMESS;
    for (long unsigned int i = 0; i < messageRecv.size(); i++)
    {
        if (messageRecv[i] == '\n')
        {
            parseMess.push_back(tempMESS);
            tempMESS.clear();
        }
        else
        {
            tempMESS += messageRecv[i];
        }
    }

    char trueDelim = '\n';
    if (parseMess.size() >= 4)
    {
        tp.packageNUM = std::stoi(parseMess[0]);
        tp.delim = parseMess[1];
        tp.length = std::stoi(parseMess[2]);
        tp.type = std::stoi(parseMess[3]);
        if (tp.delim == "\\n")
        {
            trueDelim = '\n';
            tp.delim = trueDelim;
        }
    }
    else
    {
        std::cerr << "Error in vector creation" << std::endl;
    }

    return tp;
}

void LISTfunct(std::string input, int clientSocket)
{

    struct TextPreset tp;
    tp.type = 3;

    if (input.size() > 4)
    {
        std::cout << "LIST detected: Do you still wish to send the longer sentence [y, n] (You will send it as a Comment)" << std::endl;
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

            if (input.size() > 1024)
            {
                roundErr = input.size() / 1024;
                tp.packageNUM = std::ceil(roundErr);
            }
            else
            {
                tp.packageNUM = 1;
            }

            TempINFO = std::to_string(tp.packageNUM) + '\n' + tp.delim + '\n' + std::to_string(tp.length) + '\n' + std::to_string(tp.type) + '\n';

            if (TempINFO.size() > 1024)
            {
                std::cout << "Error String to long for info" << std::endl;
                return;
            }
            else
            {
                send(clientSocket, TempINFO.c_str(), TempINFO.size(), 0);
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

    // tp.subject = subject;
    std::string LISTstring = "";
    LISTstring = LISTstring + "LIST" + '\n';

    tp.length = LISTstring.size();

    double roundErr;

    if (LISTstring.size() > 1024)
    {
        roundErr = LISTstring.size() / 1024;
        tp.packageNUM = std::ceil(roundErr);
    }
    else
    {
        tp.packageNUM = 1;
    }
    tp.delim = "\\n";

    std::string infoString = "";
    infoString = std::to_string(tp.packageNUM) + '\n' + tp.delim + '\n' + std::to_string(tp.length) + '\n' + std::to_string(tp.type) + '\n';

    if (infoString.size() > 1024)
    {
        std::cout << "Error String to long for info" << std::endl;
    }
    else
    {
        send(clientSocket, infoString.c_str(), infoString.size(), 0);
        char buffer[1024] = {0};
        int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[errRecv] = '\n';
        if (errRecv == -1)
        {
            std::cout << "error happened during recv from server" << std::endl;
        }
        std::string OK = "OK\n";
        std::string ERR = "ERR\n";
        if (buffer == OK)
        {
            std::cout << buffer << std::endl;
            buffer[1024] = {0};
            errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[errRecv] = '\n';
            if (errRecv == -1)
            {
                std::cout << "error happened during parse of message from server" << std::endl;
            }

            if (buffer == ERR)
            {
                std::cout << buffer << std::endl;
                return;
            }
            else
            {
                buffer[1024] = {0};
                errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
                buffer[errRecv] = '\n';
                if (errRecv == -1)
                {
                    std::cout << "error happened during parse of message from server" << std::endl;
                }
                tp = parseInfoString(tp, buffer);
                if (tp.type == 3)
                {
                    int j = tp.packageNUM;

                    if (tp.packageNUM > 1)
                    {

                        do
                        {
                            char tempBuffer[1024] = {0};
                            int tempErrRecv = recv(clientSocket, tempBuffer, sizeof(tempBuffer), 0);
                            tempBuffer[tempErrRecv] = '\n';
                            std::string tempParseMessage(buffer);
                            std::cout << buffer;
                            j--;
                        } while (j >= 1);
                    }
                    else
                    {
                        buffer[1024] = {0};
                        errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
                        buffer[errRecv] = '\n';
                        if (errRecv == -1)
                        {
                            std::cout << "error happened during parse of message from server" << std::endl;
                        }
                    }
                }
            }
        }
        else if (buffer == ERR)
        {
            std::cout << buffer << std::endl;
            return;
        }
    }
}

void SENDfunct(std::string input, int clientSocket)
{

    struct TextPreset tp;
    tp.type = 1;
    tp.length = 0;

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

            if (input.size() > 1024)
            {
                roundErr = input.size() / 1024;
                tp.packageNUM = std::ceil(roundErr);
            }
            else
            {
                tp.packageNUM = 1;
            }

            TempINFO = std::to_string(tp.packageNUM) + '\n' + tp.delim + '\n' + std::to_string(tp.length) + '\n' + std::to_string(tp.type) + '\n';

            if (TempINFO.size() > 1024)
            {
                std::cout << "Error String to long for info" << std::endl;
                return;
            }
            else
            {
                send(clientSocket, TempINFO.c_str(), TempINFO.size(), 0);
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
    double roundErr;

    if (SENDstring.size() > 1024)
    {
        roundErr = SENDstring.size() / 1024;
        tp.packageNUM = std::ceil(roundErr);
    }
    else
    {
        tp.packageNUM = 1;
    }
    tp.delim = "\\n";
    tp.length = SENDstring.size();

    std::string infoString = "";
    infoString = std::to_string(tp.packageNUM) + '\n' + tp.delim + '\n' + std::to_string(tp.length) + '\n' + std::to_string(tp.type) + '\n';

    if (infoString.size() > 1024)
    {
        std::cout << "Error String to long for info" << std::endl;
    }
    else
    {
        send(clientSocket, infoString.c_str(), infoString.size(), 0);
        char buffer[1024] = {0};
        int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[errRecv] = '\n';
        if (errRecv == -1)
        {
            std::cout << "error happened during recv from server" << std::endl;
        }
        std::string OK = "OK\n";
        std::string ERR = "ERR\n";
        if (buffer == OK)
        {
            std::cout << buffer << std::endl;
            std::cout << "Send Message" << std::endl;

            std::string clippedString = "";

            if(tp.packageNUM > 1){
                int i = 1;
            do
            {

                int startIndex = (i - 1) * 1023;
                int endIndex = std::min(startIndex + 1023, tp.length);

                clippedString = SENDstring.substr(startIndex, endIndex - startIndex);

                send(clientSocket, clippedString.c_str(), clippedString.size(), 0);

                i++;
            } while (i <= tp.packageNUM + 1);
            }else{
                send(clientSocket, SENDstring.c_str(), SENDstring.size(), 0);
            }

            

            buffer[1024] = {0};
            errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[errRecv] = '\n';
            if (errRecv == -1)
            {
                std::cout << "error happened during parse of message from server" << std::endl;
            }
            if (buffer == OK)
            {
                std::cout << buffer << std::endl;
                std::cout << "Message recieved and saved" << std::endl;
            }
            else
            {
                std::cout << buffer << std::endl;
                return;
            }
        }
        else if (buffer == ERR)
        {
            std::cout << buffer << std::endl;
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
    char buffer[1024] = {0};
    int errRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[errRecv] = '\n';
    if (errRecv == -1)
    {
        std::cout << "error happened during recv from server" << std::endl;
    }
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
            LISTfunct(input, clientSocket);
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