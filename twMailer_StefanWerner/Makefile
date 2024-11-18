CC=g++
CFLAGS=-g -Wall -Wextra -Werror -O -std=c++17 -pthread

rebuild: clean all
all: ./bin/server ./bin/client

prepare:
	mkdir -p obj bin

clean:
	clear
	rm -f bin/* obj/*

# main programms
./obj/client.o: twmailer-client.cpp | prepare
	${CC} ${CFLAGS} -o obj/client.o twmailer-client.cpp -c

./obj/server.o: twmailer-server.cpp | prepare
	${CC} ${CFLAGS} -o obj/server.o twmailer-server.cpp -c

# Classes
./obj/parseClass.o: classes/parseClass.cpp | prepare
	${CC} ${CFLAGS} -o obj/parseClass.o classes/parseClass.cpp -c

./obj/fileHandeling.o: classes/fileHandeling.cpp | prepare
	${CC} ${CFLAGS} -o obj/fileHandeling.o classes/fileHandeling.cpp -c

./obj/messageClass.o: classes/messageClass.cpp | prepare
	${CC} ${CFLAGS} -o obj/messageClass.o classes/messageClass.cpp -c

./obj/userInputClass.o: classes/userInputClass.cpp | prepare
	${CC} ${CFLAGS} -o obj/userInputClass.o classes/userInputClass.cpp -c

# headers
./obj/presetStruct.o: headers/PresetStruct.cpp | prepare
	${CC} ${CFLAGS} -o obj/presetStruct.o headers/PresetStruct.cpp -c

# binary
./bin/server: ./obj/presetStruct.o ./obj/messageClass.o ./obj/fileHandeling.o ./obj/parseClass.o ./obj/server.o | prepare
	${CC} ${CFLAGS} -o bin/server obj/messageClass.o obj/presetStruct.o obj/fileHandeling.o obj/parseClass.o obj/server.o

./bin/client: ./obj/presetStruct.o ./obj/userInputClass.o ./obj/messageClass.o ./obj/fileHandeling.o ./obj/parseClass.o ./obj/client.o | prepare
	${CC} ${CFLAGS} -o bin/client obj/userInputClass.o obj/messageClass.o obj/presetStruct.o obj/fileHandeling.o obj/parseClass.o obj/client.o
