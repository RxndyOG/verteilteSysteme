#include <stdio.h>
#include <unistd.h> // für getopt()

int main(int argc, char* argv[]){

    int command;
    char* configDatei;
    while((command = getopt(argc, argv, "qc:")) != EOF){ // EOF == -1
        switch(command){
        case '?':
            printf("%s [-q] [-c <configDatei>] <verzeichnis>\n", argv[0]);
            break;
        case 'q':
            printf("q command\n");
            break;
        case 'c':
            printf("c command\n");
            configDatei = optarg;       // optarg wird rot angezeigt compiled jedoch. (IDE fehler)
            printf("argument from c: %s\n", configDatei);
        break;
        default:
            break;
        }
    }

    printf("Verzeichnis: %s\n", argv[optind]); // optind wird rot angezeigt compiled jedoch. (IDE fehler)

    return 0;
}