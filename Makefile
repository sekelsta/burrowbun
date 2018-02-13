CC = g++
# For debugging use -Og -g
# For going fast use -O3
CXXFLAGS = -std=c++14 -Wall -D_GLIBCXX_DEBUG -D_LIBCXX_DEBUG_PEDANTIC -Og -g
#CXXFLAGS = -std=c++14 -Wall -O3
LINKER_FLAGS = -lSDL2 -lSDL2_image
NOISE_FLAGS = -I/usr/include/libnoise -L/usr/lib -lnoise

# Defining variables that say which files all the headers include
Action.hh.d = Action.hh $(Sprite.hh.d)
AllTheItems.hh.d = AllTheItems.hh $(Item.hh.d) $(Tile.hh.d) $(MapHelpers.hh.d)
Animation.hh.d = Animation.hh $(Sprite.hh.d) $(SpriteBase.hh.d) $(Rect.hh.d)
Boulder.hh.d = Boulder.hh $(Tile.hh.d)
Collider.hh.d = Collider.hh $(Tile.hh.d) $(Map.hh.d) $(Movable.hh.d) \
        $(Rect.hh.d) $(DroppedItem.hh.d)
Damage.hh.d = Damage.hh
DroppedItem.hh.d = DroppedItem.hh $(Item.hh.d) $(Movable.hh.d)
Entity.hh.d = Entity.hh $(Movable.hh.d) $(Stat.hh.d) $(Damage.hh.d) \
        $(Animation.hh.d)
EventHandler.hh.d = EventHandler.hh
Game.hh.d = Game.hh $(WindowHandler.hh.d)
Hotbar.hh.d = Hotbar.hh $(Sprite.hh.d) $(UIHelpers.hh.d) $(Action.hh.d)
Inventory.hh.d = Inventory.hh $(Sprite.hh.d) $(UIHelpers.hh.d) \
        $(Light.hh.d) $(Item.hh.d)
Item.hh.d = Item.hh $(Action.hh.d)
Light.hh.d = Light.hh
Mapgen.hh.d = Mapgen.hh $(Tile.hh.d) $(MapHelpers.hh.d) $(Map.hh.d)
MapHelpers.hh.d = MapHelpers.hh $(Tile.hh.d) $(Light.hh.d)
Map.hh.d = Map.hh $(Tile.hh.d) $(MapHelpers.hh.d)
Movable.hh.d = Movable.hh $(Sprite.hh.d) $(Damage.hh.d) $(Rect.hh.d)
Player.hh.d = Player.hh $(Entity.hh.d) $(Hotbar.hh.d) $(Inventory.hh.d) \
        $(UIHelpers.hh.d)
Rect.hh.d = Rect.hh
Renderer.hh.d = Renderer.hh $(Light.hh.d)
Sprite.hh.d = Sprite.hh $(Texture.hh.d) $(SpriteBase.hh.d)
SpriteBase.hh.d = SpriteBase.hh $(Texture.hh.d) $(Light.hh.d)
Stat.hh.d = Stat.hh
Texture.hh.d = Texture.hh $(Light.hh.d) $(Renderer.hh.d)
Tile.hh.d = Tile.hh $(Sprite.hh.d) $(Movable.hh.d) $(Light.hh.d) $(Damage.hh.d)
UIHelpers.hh.d = UIHelpers.hh $(Light.hh.d) $(Stat.hh.d) $(Sprite.hh.d)
WindowHandler.hh.d = WindowHandler.hh $(Sprite.hh.d) $(Renderer.hh.d) \
        $(Movable.hh.d) $(UIHelpers.hh.d)

all : burrowbun

burrowbun : main.o Map.o Mapgen.o Tile.o WindowHandler.o EventHandler.o \
        Movable.o DroppedItem.o Game.o \
        Player.o Collider.o Hotbar.o Entity.o Inventory.o Item.o \
        Sprite.o AllTheItems.o Boulder.o MapHelpers.o Light.o Stat.o \
        UIHelpers.o Damage.o Texture.o Renderer.o Animation.o SpriteBase.o
	$(CC) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(NOISE_FLAGS) $(LINKER_FLAGS)

main.o : main.cc $(Game.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(NOISE_FLAGS) $(LINKER_FLAGS)

Game.o : Game.cc $(Game.hh.d) $(Tile.hh.d) $(Map.hh.d) $(Mapgen.hh.d) \
        $(EventHandler.hh.d) $(Movable.hh.d) $(Entity.hh.d) $(Player.hh.d) \
        $(Collider.hh.d) $(Hotbar.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(NOISE_FLAGS) $(LINKER_FLAGS)

Animation.o : Animation.cc $(Animation.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Map.o : Map.cc $(Map.hh.d) $(Boulder.hh.d) version.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Tile.o : Tile.cc $(Tile.hh.d) $(Map.hh.d) $(Movable.hh.d) filepaths.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

WindowHandler.o : WindowHandler.cc $(WindowHandler.hh.d) $(Light.hh.d) \
        $(Tile.hh.d) $(Map.hh.d) $(Hotbar.hh.d) $(Player.hh.d) \
        $(UIHelpers.hh.d) $(Sprite.hh.d) $(Inventory.hh.d) $(Action.hh.d) \
        $(Rect.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

EventHandler.o : EventHandler.cc $(EventHandler.hh.d) $(WindowHandler.hh.d) \
        $(Player.hh.d) $(Hotbar.hh.d) $(UIHelpers.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

Movable.o : Movable.cc $(Movable.hh.d) filepaths.hh $(Renderer.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

Player.o : Player.cc $(Player.hh.d) $(Item.hh.d) $(AllTheItems.hh.d) 
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Collider.o : Collider.cc $(Collider.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Mapgen.o : Mapgen.cc $(Mapgen.hh.d) version.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(NOISE_FLAGS)

Hotbar.o : Hotbar.cc $(Hotbar.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Entity.o : Entity.cc $(Entity.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Inventory.o : Inventory.cc $(Inventory.hh.d) filepaths.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Item.o : Item.cc $(Item.hh.d) filepaths.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

AllTheItems.o : AllTheItems.cc $(AllTheItems.hh.d) $(Player.hh.d) $(Map.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Sprite.o : Sprite.cc $(Sprite.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

SpriteBase.o : SpriteBase.cc $(SpriteBase.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Boulder.o : Boulder.cc $(Boulder.hh.d) $(Map.hh.d) $(Tile.hh.d) \
        $(MapHelpers.hh.d) $(Rect.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

MapHelpers.o : MapHelpers.cc $(MapHelpers.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Light.o : Light.cc $(Light.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Stat.o : Stat.cc $(Stat.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

UIHelpers.o : UIHelpers.cc $(UIHelpers.hh.d) filepaths.hh $(Renderer.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Damage.o: Damage.cc $(Damage.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Texture.o: Texture.cc $(Texture.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

Renderer.o: Renderer.cc $(Renderer.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

DroppedItem.o: DroppedItem.cc $(DroppedItem.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

tests : collider_tests.o
	$(CC) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(NOISE_FLAGS) $(LINKER_FLAGS)

collider_tests.o : collider_tests.cc Collider.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

clean :
	rm -f *.o *.gch *~

.PHONY : all clean

