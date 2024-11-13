#include "myheader.h" // alle notwendigen includes
#define BUF 1024
#define PORT 6543

int main(void){
    int cs, ns;
    socklen_t addrlen;
    char buffer[BUF];
    struct sockaddr_in addr, cl;
    pid_t pid;

    cs = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = "127.0.0.1";
    addr.sin_port = PORT;

    bind (cs, (struct sockaddr *)&addr, sizeof(addr));
    listen (cs, 5);
    
    addrlen = sizeof(struct sockaddr_in);

    while(1){
        printf("Waiting for connection ... \n");
        ns = accept(cs, (struct sockaddr*)&cl, addrlen);
        if((pid = fork()) == 0){
            do{
                recv(ns, buffer, BUF, 0);
                printf("Message received: %s\n", buffer);
            }while(strncmp(buffer, "quit", 4));
            close(cs);
        }
    }
    close(cs);
    return 0;
}