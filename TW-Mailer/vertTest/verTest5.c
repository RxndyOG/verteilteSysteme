
// unnamed PIPE test

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char* argv[]){

    int fileDescriptor[2];  //fileDescriptor[0] == read end (wo gelesen werden soll), fileDescriptor[1] == write end (wohin geschrieben werden soll)
    if(pipe(fileDescriptor) == -1){
        printf("Error opening Pipe");
        return 1;
    }

    pid_t pid = fork();

    if(pid == 0){
        close(fileDescriptor[0]);   // read schließen wenn nicht gelesen werden muss
        int sendData = 1;
        write(fileDescriptor[1], &sendData, sizeof(int));
        close(fileDescriptor[1]);   // write schließen wenn fertig geschrieben
    }else{
        close(fileDescriptor[1]);   // write schließen wenn nicht geschrieben werden muss
        int recvData = 0;
        read(fileDescriptor[0], &recvData, sizeof(int));
        close(fileDescriptor[0]);   // read schließen wenn fertig gelesen wurde
        printf("data from child: %d\n", recvData);
    }

    return 0;
}