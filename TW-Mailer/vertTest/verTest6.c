#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h> // mkfifo()

int main() {
    const char *fifo_path = "/tmp/myfifo";
    mkfifo(fifo_path, 0666); // path, mode

    if (fork() == 0) { 
        int sendData = 1;
        int fd = open(fifo_path, O_WRONLY);
        write(fd, &sendData, sizeof(int));
        close(fd);
    } else {
        int recvData = 0;
        int fd = open(fifo_path, O_RDONLY);
        read(fd, &recvData, sizeof(int));
        printf("Nachricht vom Kind: %d\n", recvData);
        close(fd);
        unlink(fifo_path);
    }

    return 0;
}