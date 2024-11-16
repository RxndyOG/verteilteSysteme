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

./obj/sendMESSclass.o: classes/sendMESSclass.cpp | prepare
	${CC} ${CFLAGS} -o obj/sendMESSclass.o classes/sendMESSclass.cpp -c

./obj/basicFunktions.o: classes/basicFunktions.cpp | prepare
	${CC} ${CFLAGS} -o obj/basicFunktions.o classes/basicFunktions.cpp -c

./obj/FileHandeling.o: classes/FileHandeling.cpp | prepare
	${CC} ${CFLAGS} -o obj/FileHandeling.o classes/FileHandeling.cpp -c

./obj/initializeClass.o: classes/initializeClass.cpp | prepare
	${CC} ${CFLAGS} -o obj/initializeClass.o classes/initializeClass.cpp -c

./obj/userInputClass.o: classes/userInputClass.cpp | prepare
	${CC} ${CFLAGS} -o obj/userInputClass.o classes/userInputClass.cpp -c

# headers
./obj/presetsStruct.o: headers/PresetsStruct.cpp | prepare
	${CC} ${CFLAGS} -o obj/presetsStruct.o headers/PresetsStruct.cpp -c

# binary
./bin/server: ./obj/server.o ./obj/presetsStruct.o ./obj/basicFunktions.o ./obj/FileHandeling.o ./obj/initializeClass.o ./obj/sendMESSclass.o ./obj/parseClass.o | prepare
	${CC} ${CFLAGS} -o bin/server obj/parseClass.o obj/basicFunktions.o obj/FileHandeling.o obj/initializeClass.o obj/sendMESSclass.o obj/server.o obj/presetsStruct.o

./bin/client: ./obj/client.o ./obj/parseClass.o ./obj/userInputClass.o ./obj/basicFunktions.o ./obj/FileHandeling.o ./obj/initializeClass.o ./obj/sendMESSclass.o ./obj/parseClass.o | prepare
	${CC} ${CFLAGS} -o bin/client obj/client.o obj/basicFunktions.o obj/userInputClass.o obj/FileHandeling.o obj/initializeClass.o obj/sendMESSclass.o obj/parseClass.o obj/presetsStruct.o