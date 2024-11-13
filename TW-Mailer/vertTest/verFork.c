
#include <sys/types.h> // pid_t
#include <unistd.h> // fork()
#include <stdio.h>
#include <stdlib.h>

int main(){

    pid_t  pid;

    // parent = 1 -> child = 0; child ->  PID = 0; child - child

    // -> child -> PID = 0; parent -> PID > 0; -1 error

    int a,b;


    switch(pid = fork()){   // erstellt einen 2 process 
        case -1:
        //error
            break;
        case 0:
        //child
            a = 5;
            b = 3;
            printf("%d\n", a + b);
            exit(0);
            break;
        default:
        //parent
            a = 10;
            b = 0;
            printf("%d\n", a + b);
            break;       
    }

    return 0;
}