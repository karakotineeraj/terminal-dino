CFLAGS := -Wall -pedantic -std=c++11

main:
	g++ -o bin/main src/main.cpp -lncurses ${CFLAGS}
