#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> // einfügen damit das programm kompiliert
#include <sys/wait.h> // einfügen für wait()

int main(){

    pid_t pid;
    int a =0;
    int b =0;

    pid = fork();
    switch(pid){
        case -1:
            printf("fork failed"); return -1; break;
        case 0:
            a = 3;
            b= 5;
            printf("1: a: %d b: %d\n", a, b);

            exit(0);
            break;
        default:
            b = a+1;
            printf("2: a: %d b: %d\n", a, b);
            // einfügen damit es programmierrichtlinien unterstützt
            wait(NULL);
            printf("kindprozess beendet\n");
            break;
    }
    a++;
    b--;
    printf("3: a: %d b: %d\n", a, b);

    return 0;
}