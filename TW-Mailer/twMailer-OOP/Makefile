CC=g++
CFLAGS=-g -Wall -Wextra -Werror -O -std=c++17 -pthread

rebuild: clean all
all: ./bin/server ./bin/client

clean:
	clear
	rm -f bin/* obj/*

./obj/client.o: twmailer-client.cpp
	${CC} ${CFLAGS} -o obj/client.o twmailer-client.cpp -c

./obj/server.o: twmailer-server.cpp
	${CC} ${CFLAGS} -o obj/server.o twmailer-server.cpp -c

./obj/ServerClass.o: ServerClass.cpp
	${CC} ${CFLAGS} -o obj/ServerClass.o ServerClass.cpp -c

./obj/ClientClass.o: ClientClass.cpp
	${CC} ${CFLAGS} -o obj/ClientClass.o ClientClass.cpp -c



./obj/UserInputClass.o: UserInputClass.cpp
	${CC} ${CFLAGS} -o obj/UserInputClass.o UserInputClass.cpp -c

./obj/ErrorHandling.o: ErrorHandling.cpp
	${CC} ${CFLAGS} -o obj/ErrorHandling.o ErrorHandling.cpp -c

./obj/FileHandling.o: FileHandling.cpp
	${CC} ${CFLAGS} -o obj/FileHandling.o FileHandling.cpp -c

./obj/BasicSocketFunction.o: BasicSocketFunction.cpp
	${CC} ${CFLAGS} -o obj/BasicSocketFunction.o BasicSocketFunction.cpp -c



./bin/server: ./obj/server.o ./obj/ServerClass.o ./obj/FileHandling.o ./obj/ErrorHandling.o ./obj/BasicSocketFunction.o
	${CC} ${CFLAGS} -o bin/server obj/ServerClass.o obj/FileHandling.o obj/ErrorHandling.o obj/BasicSocketFunction.o obj/server.o

./bin/client: ./obj/client.o ./obj/ClientClass.o ./obj/UserInputClass.o ./obj/FileHandling.o ./obj/ErrorHandling.o ./obj/BasicSocketFunction.o
	${CC} ${CFLAGS} -o bin/client obj/ClientClass.o obj/UserInputClass.o obj/FileHandling.o obj/ErrorHandling.o obj/BasicSocketFunction.o obj/client.o