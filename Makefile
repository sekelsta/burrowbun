CC = g++
# For debugging use -Og -g
# For going fast use -O2 or -O3
CXXFLAGS = -std=c++14 -Wall -Og -g
LINKER_FLAGS = -lSDL2 -lSDL2_image

all : main

main : main.o Map.o Tile.o WindowHandler.o EventHandler.o Movable.o \
        Player.o Collider.o
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -o $@ $(LDFLAGS)

Map.o : Map.cc Map.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Tile.o : Tile.cc Tile.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

WindowHandler.o : WindowHandler.cc WindowHandler.hh
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -c $(LDFLAGS)

EventHandler.o : EventHandler.cc EventHandler.hh
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -c $(LDFLAGS)

Movable.o : Movable.cc Movable.hh
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -c $(LDFLAGS)

Player.o : Player.cc Player.hh
	$(CC) $(CXXFLAGS) $(LINKER_FLAGS) $^ -c $(LDFLAGS)

Collider.o : Collider.cc Collider.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

clean :
	rm -f *.o *.gch *~ *.world

.PHONY : all clean

