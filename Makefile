CC = g++
# For debugging use -Og -g
# For going fast use -O2 or -O3
CXXFLAGS = -std=c++14 -Wall -D_GLIBCXX_DEBUG -D_LIBCXX_DEBUG_PEDANTIC -Og -g
LINKER_FLAGS = -lSDL2 -lSDL2_image
NOISE_FLAGS = -I/usr/include/libnoise -L/usr/lib -lnoise

all : main

main : main.o Map.o World.o Tile.o WindowHandler.o EventHandler.o Movable.o \
        Player.o Collider.o Hotbar.o Entity.o Inventory.o Item.o Action.o \
        Sprite.o AllTheItems.o Boulder.o MapHelpers.o
	$(CC) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(NOISE_FLAGS) $(LINKER_FLAGS)

main.o : main.cc World.hh Tile.hh Map.hh WindowHandler.hh EventHandler.hh \
        Collider.hh Hotbar.hh Movable.hh Player.hh Light.hh MapHelpers.hh \
        Sprite.hh UIHelpers.hh Entity.hh Inventory.hh Player.hh \
        Item.hh Action.hh AllTheItems.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

Map.o : Map.cc Map.hh Tile.hh MapHelpers.hh Light.hh Movable.hh Sprite.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Tile.o : Tile.cc Tile.hh Map.hh Sprite.hh Movable.hh json.hpp
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

WindowHandler.o : WindowHandler.cc WindowHandler.hh Tile.hh Map.hh Movable.hh \
        Light.hh MapHelpers.hh Hotbar.hh Sprite.hh Inventory.hh Action.hh \
        Item.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

EventHandler.o : EventHandler.cc EventHandler.hh WindowHandler.hh Player.hh \
        Movable.hh Hotbar.hh UIHelpers.hh Entity.hh Inventory.hh \
        Action.hh Item.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

Movable.o : Movable.cc Movable.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

Player.o : Player.cc Player.hh Movable.hh Hotbar.hh UIHelpers.hh \
        Light.hh Entity.hh Inventory.hh Action.hh Item.hh AllTheItems.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Collider.o : Collider.cc Collider.hh Tile.hh Map.hh Movable.hh Sprite.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

World.o : World.cc World.hh Tile.hh MapHelpers.hh Sprite.hh Movable.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(NOISE_FLAGS)

Hotbar.o : Hotbar.cc Hotbar.hh Sprite.hh UIHelpers.hh Action.hh Item.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Entity.o : Entity.cc Entity.hh Movable.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Inventory.o : Inventory.cc Inventory.hh Light.hh UIHelpers.hh Sprite.hh \
        Action.hh Item.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Item.o : Item.cc Item.hh Sprite.hh Inventory.hh Action.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Action.o : Action.cc Action.hh Sprite.hh Inventory.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

AllTheItems.o : AllTheItems.cc AllTheItems.hh Item.hh Action.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Sprite.o : Sprite.cc Sprite.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Boulder.o : Boulder.cc Boulder.hh Map.hh Tile.hh MapHelpers.hh Sprite.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

MapHelpers.o : MapHelpers.cc MapHelpers.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

clean :
	rm -f *.o *.gch *~ *.world

.PHONY : all clean

