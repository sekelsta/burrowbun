CC = g++
CXXFLAGS = -std=c++14 -Wall
LINKER_FLAGS = -lSDL2

all : main

main : main.o Map.o Tile.o
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -o $@ $(LDFLAGS)

clean :
	rm -f main *.o *~ *.world

.PHONY : all clean

