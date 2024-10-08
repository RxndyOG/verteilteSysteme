output: myfind.o
	g++ myfind.o -o myfind 

myfind.o: myfind.cpp
	g++ -std=c++17 -Wall -Werror -c myfind.cpp

.PHONY: clean
clean:
	rm -f *.o myfind
