CC = g++
CXXFLAGS = -std=c++14 -Wall
LINKER_FLAGS = -lSDL2

all : main

main : main.o Map.o Tile.o
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -o $@ $(LDFLAGS)

Map.o : Map.cc Map.hh
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -c $(LDFLAGS)

Tile.o : Tile.cc Tile.hh
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -c $(LDFLAGS)

clean :
	rm -f main *.o *~ *.world

.PHONY : all clean

