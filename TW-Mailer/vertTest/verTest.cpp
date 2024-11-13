#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> //einfügen für exit()
#include <sys/wait.h> //einfügen for wait()

int main(){

    pid_t pid;
    int a = 12;
    int b = 5;

    pid = fork();
    switch(pid){
        case -1:
            printf("fork failed"); return -1; break;
        default:
        sleep(1);
            a=3;
            b=5;
            printf("1: a: %d b: %d\n", a, b);
            // einfügen damit es programmierrichtlinien unterstützt
            wait(NULL);
            printf("kindprozess beendet\n");
            break;
        case 0:
            sleep(3);
            b = a+1;
            printf("2: a: %d b: %d\n", a, b);

            //einfügen damit es programmierrichtlinien entspricht
            exit(EXIT_SUCCESS);
            break;
    }
    a++;
    b--;
    printf("3: a: %d b: %d\n", a, b);

    return 0;
}