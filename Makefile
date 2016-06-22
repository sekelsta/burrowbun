CC = g++
CXXFLAGS = -std=c++14 -Wall
LINKER_FLAGS = -lSDL2 -lSDL2_image

all : main

main : main.o Map.o Tile.o WindowHandler.o
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -o $@ $(LDFLAGS)

Map.o : Map.cc Map.hh
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -c $(LDFLAGS)

Tile.o : Tile.cc Tile.hh
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -c $(LDFLAGS)

WindowHandler.o : WindowHandler.cc WindowHandler.hh
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -c $(LDFLAGS)

clean :
	rm -f main *.o *.gch *~ *.world

.PHONY : all clean

