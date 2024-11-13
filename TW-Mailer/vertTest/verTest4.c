#include <stdio.h>
#include <unistd.h> // getopt();

int main(int argc, char* argv[]){

int command;
int xSET = 0;   // in cpp bool is prefered // verhindern das mehrmals ausgeführt wird
int vSET = 0;
int fSET = 0;

    while((command = getopt(argc, argv, "xvf:")) != EOF){ //EOF == -1
        switch(command){
            case '?':
                printf("Usage: %s [-x] [-v] [-f <SOURCEFILE>]\n", argv[0]);
                break;
            case 'x':
                if(xSET == 0){  //benutzt fürs verhindern das eine Option mehrfach ausgeführt wird
                    printf("X command\n");
                }
                xSET = 1;
                break;
            case 'v':
                if(vSET == 0){
                    printf("V command\n");
                }
                vSET = 1;
                break;
            case 'f':
                if(fSET == 0){
                    printf("F command\n");
                    printf("F argument: %s\n", optarg);
                }
                fSET = 1;
                break;
            default:
                break;
        }
    }
}