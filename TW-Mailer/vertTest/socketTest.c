
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 1001
#define BUFFSIZE 1024

int main()
{

    int serverSocket, clientSocket;
    socklen_t addrlen;


    struct sockaddr_in addr, clientAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        printf("error");
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    // addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, "127.0.0.1", addr.sin_addr.s_addr);

    int err = bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr));
    if (err == -1)
    {
        printf("error");
        return -1;
    }

    err = listen(serverSocket, 5);
    if (err == -1)
    {
        printf("error");
        return -1;
    }

    addrlen = sizeof(struct sockaddr_in);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrlen);
    if (clientSocket == -1)
    {
        printf("error");
        return -1;
    }

    while (1)
    {
        char buffer[BUFFSIZE] = {};
        err = recv(clientSocket, buffer, BUFFSIZE, 0);
        buffer[err] = '\0';     // ganz ganz wichtig 5er wenn nicht da!!!!!!
        if(err == -1){
            printf("error");
        }

        break;
    }

    return 0;
}