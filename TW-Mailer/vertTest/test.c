#include <string.h>
#include <stdio.h>

int main(){

    char buffer[1024];

    strncpy(buffer, "hallo", 5);


    printf("%ld\n", strlen(buffer));
    printf("%ld\n", sizeof(buffer));


    return 0;
}