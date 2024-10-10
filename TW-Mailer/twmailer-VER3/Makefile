CC=g++
CFLAGS=-g -Wall -Wextra -Werror -O -std=c++17 -pthread

rebuild: clean all
all: ./server ./client

clean:
	clear
	rm -f ./server ./client

client.o: twmailer-client.cpp
	${CC} ${CFLAGS} -o client.o twmailer-client.cpp -c

server.o: twmailer-server.cpp
	${CC} ${CFLAGS} -o server.o twmailer-server.cpp -c

server: ./server.o
	${CC} ${CFLAGS} -o server server.o

client: ./client.o
	${CC} ${CFLAGS} -o client client.o