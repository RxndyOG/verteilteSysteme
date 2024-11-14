#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cmath>

int _blockSIZE = 1024;

struct txtStruct {
    std::string subject;
    std::string sender;
    std::string text;
};

enum type {
    SEND = 1,
    READ = 2,
    LIST = 3,
    DEL = 4,
    QUIT = 5,
    COMMENT = 0,
};

struct INFOStruct {
    int argument;
    std::string username;
    int textLength;
    int packageNUM;
    std::string INFOstring;
};
std::string recvFunctBasic(int clientSocket, int &ERR) {
    char buffer[1024] = {0};
    int errRcv = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (errRcv <= 0) {
        ERR = -1;
        std::cerr << "Error or disconnection in receiving single packet." << std::endl;
        return "";
    }
    buffer[errRcv] = '\0';
    ERR = 0;
    return std::string(buffer);
}
void sendFunctBasic(const std::string &arg, int clientSocket) {
    send(clientSocket, arg.c_str(), arg.size(), 0);
}

void SENDoptionParse(INFOStruct *is, txtStruct *ts, int clientSocket) {
    std::vector<std::string> argArray;
    int ERR = 0;
    std::string completeSTR;

    while (is->packageNUM > 0) {
        std::string tmpString = recvFunctBasic(clientSocket, ERR);
        if (ERR == -1) return;
        completeSTR.append(tmpString);
        is->packageNUM--;
    }

    std::cout << completeSTR << std::endl;

    std::string tmpString;
    for (char ch : completeSTR) {
        if (ch == '\n') {
            argArray.push_back(tmpString);
            tmpString.clear();
        } else {
            tmpString += ch;
        }
    }

    if (argArray.size() >= 3) {
        ts->sender = argArray[0];
        ts->subject = argArray[1];
        ts->text = completeSTR.substr(completeSTR.find(argArray[1]) + argArray[1].size() + 1);
    }
}

int calcDirection(INFOStruct *is, int clientSocket) {
    txtStruct *ts = new txtStruct();
    switch (is->argument) {
        case SEND:
            std::cout << "SEND from client" << std::endl;
            SENDoptionParse(is, ts, clientSocket);
            break;
        case READ:
        case LIST:
        case DEL:
        case QUIT:
            delete ts;
            return QUIT;
    }
    delete ts;
    return COMMENT;
}

void parseINFOstring(INFOStruct *is) {
    std::vector<std::string> argArray;
    std::string tmpString;

    for (char ch : is->INFOstring) {
        if (ch == '\n') {
            argArray.push_back(tmpString);
            tmpString.clear();
        } else {
            tmpString += ch;
        }
    }

    if (argArray.size() >= 4) {
        try {
            is->argument = std::stoi(argArray[0]);
            is->username = argArray[1];
            is->textLength = std::stoi(argArray[2]);
            is->packageNUM = std::stoi(argArray[3]);
        } catch (const std::invalid_argument &e) {
            std::cerr << "Invalid conversion in parseINFOstring: " << e.what() << std::endl;
        } catch (const std::out_of_range &e) {
            std::cerr << "Out of range conversion in parseINFOstring: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "ERROR: INFOstring format is incorrect." << std::endl;
    }
}

int recvAndParseInfoString(INFOStruct *is, int clientSocket) {
    int ERR = 0;
    is->INFOstring = recvFunctBasic(clientSocket, ERR);
    if (ERR != -1) {
        sendFunctBasic("OK\n", clientSocket);
        parseINFOstring(is);
        return is->argument;
    }
    return ERR;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(std::stoi(argv[1]));
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    int reuse = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return 2;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening" << std::endl;
        close(serverSocket);
        return 3;
    }

    std::cout << "Server listening on port " << argv[1] << std::endl;

    sockaddr_in clientAddress;
    socklen_t clientSize = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientSize);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection" << std::endl;
        close(serverSocket);
        return 4;
    }

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddress.sin_port);
    std::cout << "Accepted connection from " << clientIP << ":" << clientPort << std::endl;

    send(clientSocket, "Hello from Server", sizeof("Hello from Server"), 0);

    int cancell;

    do {
        INFOStruct *is = new INFOStruct();
        cancell = recvAndParseInfoString(is, clientSocket);
        if (cancell != -1) {
            cancell = calcDirection(is, clientSocket);
        }
        delete is;
    } while (cancell != QUIT);

    std::cout << "Client closed server" << std::endl;

    close(clientSocket);
    close(serverSocket);

    return 0;
}
