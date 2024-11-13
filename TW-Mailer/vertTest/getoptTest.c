#include <stdio.h>
#include <unistd.h>

// arg = argument;

// c = count

// argc = argument count -> int anzahl der argumente

// v = vector

// argv = argument vector -> vector argument

// * argv[] = vector<argv>

int main(int argc, char *argv[])
{

    int command;

    while ((command = getopt(argc, argv, "qc:")) != EOF)
    {
        switch (command)
        {
        case '?':
            printf("error\n");
            break;
        case 'q':
            printf("q option\n");
            break;
        case 'c':
            printf("c option\n");
            printf("%s\n", optarg);
            break;
        default:
            break;
        }
    }

    printf("%s\n", argv[optind]);#include <stdio.h>
#include <unistd.h>

// arg = argument;

// c = count

// argc = argument count -> int anzahl der argumente

// v = vector

// argv = argument vector -> vector argument

// * argv[] = vector<argv>

int main(int argc, char *argv[])
{

    int command;

    while ((command = getopt(argc, argv, "qc:")) != EOF)
    {
        switch (command)
        {
        case '?':
            printf("error\n");
            break;
        case 'q':
            printf("q option\n");
            break;
        case 'c':
            printf("c option\n");
            printf("%s\n", optarg);
            break;
        default:
            break;
        }
    }

    printf("%s\n", argv[optind]);

}

}