CC = g++
# For debugging use -Og -g
# For going fast use -O3
CXXFLAGS = -std=c++14 -Wall -D_GLIBCXX_DEBUG -D_LIBCXX_DEBUG_PEDANTIC -Og -g
#CXXFLAGS = -std=c++14 -Wall -O3
LINKER_FLAGS = -lSDL2 -lSDL2_image
NOISE_FLAGS = -I/usr/include/libnoise -L/usr/lib -lnoise

# what folders
SRCDIR = src/
OBJDIR = obj/
BINDIR = ./

# Defining variables that say which files all the headers include
Action.hh.d = $(SRCDIR)Action.hh $(Sprite.hh.d)
AllTheItems.hh.d = $(SRCDIR)AllTheItems.hh $(Item.hh.d) $(Tile.hh.d) $(MapHelpers.hh.d)
Animation.hh.d = $(SRCDIR)Animation.hh $(Sprite.hh.d) $(SpriteBase.hh.d) $(Rect.hh.d)
Boulder.hh.d = $(SRCDIR)Boulder.hh $(Tile.hh.d)
Collider.hh.d = $(SRCDIR)Collider.hh $(Tile.hh.d) $(Map.hh.d) $(Movable.hh.d) \
        $(Rect.hh.d) $(DroppedItem.hh.d)
Damage.hh.d = $(SRCDIR)Damage.hh
DroppedItem.hh.d = $(SRCDIR)DroppedItem.hh $(Item.hh.d) $(Movable.hh.d)
Entity.hh.d = $(SRCDIR)Entity.hh $(Movable.hh.d) $(Stat.hh.d) $(Damage.hh.d) \
        $(Animation.hh.d)
EventHandler.hh.d = $(SRCDIR)EventHandler.hh
Game.hh.d = $(SRCDIR)Game.hh $(WindowHandler.hh.d)
Hotbar.hh.d = $(SRCDIR)Hotbar.hh $(Sprite.hh.d) $(UIHelpers.hh.d) $(Action.hh.d)
Inventory.hh.d = $(SRCDIR)Inventory.hh $(Sprite.hh.d) $(UIHelpers.hh.d) \
        $(Light.hh.d) $(Item.hh.d)
Item.hh.d = $(SRCDIR)Item.hh $(Action.hh.d)
Light.hh.d = $(SRCDIR)Light.hh
Mapgen.hh.d = $(SRCDIR)Mapgen.hh $(Tile.hh.d) $(MapHelpers.hh.d) $(Map.hh.d)
MapHelpers.hh.d = $(SRCDIR)MapHelpers.hh $(Tile.hh.d) $(Light.hh.d)
Map.hh.d = $(SRCDIR)Map.hh $(Tile.hh.d) $(MapHelpers.hh.d)
Movable.hh.d = $(SRCDIR)Movable.hh $(Sprite.hh.d) $(Damage.hh.d) $(Rect.hh.d)
Player.hh.d = $(SRCDIR)Player.hh $(Entity.hh.d) $(Hotbar.hh.d) $(Inventory.hh.d) \
        $(UIHelpers.hh.d)
Rect.hh.d = $(SRCDIR)Rect.hh
Renderer.hh.d = $(SRCDIR)Renderer.hh $(Light.hh.d)
Sprite.hh.d = $(SRCDIR)Sprite.hh $(Texture.hh.d) $(SpriteBase.hh.d)
SpriteBase.hh.d = $(SRCDIR)SpriteBase.hh $(Texture.hh.d) $(Light.hh.d)
Stat.hh.d = $(SRCDIR)Stat.hh
Texture.hh.d = $(SRCDIR)Texture.hh $(Light.hh.d) $(Renderer.hh.d)
Tile.hh.d = $(SRCDIR)Tile.hh $(Sprite.hh.d) $(Movable.hh.d) $(Light.hh.d) $(Damage.hh.d)
UIHelpers.hh.d = $(SRCDIR)UIHelpers.hh $(Light.hh.d) $(Stat.hh.d) $(Sprite.hh.d)
WindowHandler.hh.d = $(SRCDIR)WindowHandler.hh $(Sprite.hh.d) $(Renderer.hh.d) \
        $(Movable.hh.d) $(UIHelpers.hh.d)

all : burrowbun

burrowbun : $(OBLDIR)main.o $(OBLDIR)Map.o $(OBLDIR)Mapgen.o \
        $(OBLDIR)Tile.o \
        $(OBLDIR)WindowHandler.o $(OBLDIR)EventHandler.o \
        $(OBLDIR)Movable.o $(OBLDIR)DroppedItem.o $(OBLDIR)Game.o \
        $(OBLDIR)Player.o $(OBLDIR)Collider.o $(OBLDIR)Hotbar.o \
        $(OBLDIR)Entity.o $(OBLDIR)Inventory.o $(OBLDIR)Item.o \
        $(OBLDIR)Sprite.o $(OBLDIR)AllTheItems.o $(OBLDIR)Boulder.o \
        $(OBLDIR)MapHelpers.o $(OBLDIR)Light.o $(OBLDIR)Stat.o \
        $(OBLDIR)UIHelpers.o $(OBLDIR)Damage.o $(OBLDIR)Texture.o \
        $(OBLDIR)Renderer.o $(OBLDIR)Animation.o $(OBLDIR)SpriteBase.o
	$(CC) $(CXXFLAGS) $^ -o $(OBLDIR)burrowbun $(LDFLAGS) $(NOISE_FLAGS) $(LINKER_FLAGS)

$(OBLDIR)main.o : $(SRCDIR)main.cc $(Game.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(NOISE_FLAGS) $(LINKER_FLAGS)

$(OBLDIR)Game.o : $(SRCDIR)Game.cc $(Game.hh.d) $(Tile.hh.d) $(Map.hh.d) $(Mapgen.hh.d) \
        $(EventHandler.hh.d) $(Movable.hh.d) $(Entity.hh.d) $(Player.hh.d) \
        $(Collider.hh.d) $(Hotbar.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(NOISE_FLAGS) $(LINKER_FLAGS)

$(OBLDIR)Animation.o : $(SRCDIR)Animation.cc $(Animation.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Map.o : $(SRCDIR)Map.cc $(Map.hh.d) $(Boulder.hh.d) $(SRCDIR)version.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Tile.o : $(SRCDIR)Tile.cc $(Tile.hh.d) $(Map.hh.d) $(Movable.hh.d) $(SRCDIR)filepaths.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)WindowHandler.o : $(SRCDIR)WindowHandler.cc $(WindowHandler.hh.d) $(Light.hh.d) \
        $(Tile.hh.d) $(Map.hh.d) $(Hotbar.hh.d) $(Player.hh.d) \
        $(UIHelpers.hh.d) $(Sprite.hh.d) $(Inventory.hh.d) $(Action.hh.d) \
        $(Rect.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

$(OBLDIR)EventHandler.o : $(SRCDIR)EventHandler.cc $(EventHandler.hh.d) $(WindowHandler.hh.d) \
        $(Player.hh.d) $(Hotbar.hh.d) $(UIHelpers.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

$(OBLDIR)Movable.o : $(SRCDIR)Movable.cc $(Movable.hh.d) $(SRCDIR)filepaths.hh $(Renderer.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

$(OBLDIR)Player.o : $(SRCDIR)Player.cc $(Player.hh.d) $(Item.hh.d) $(AllTheItems.hh.d) $(SRCDIR)filepaths.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Collider.o : $(SRCDIR)Collider.cc $(Collider.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Mapgen.o : $(SRCDIR)Mapgen.cc $(Mapgen.hh.d) $(SRCDIR)version.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(NOISE_FLAGS)

$(OBLDIR)Hotbar.o : $(SRCDIR)Hotbar.cc $(Hotbar.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Entity.o : $(SRCDIR)Entity.cc $(Entity.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Inventory.o : $(SRCDIR)Inventory.cc $(Inventory.hh.d) $(SRCDIR)filepaths.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Item.o : $(SRCDIR)Item.cc $(Item.hh.d) $(SRCDIR)filepaths.hh
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)AllTheItems.o : $(SRCDIR)AllTheItems.cc $(AllTheItems.hh.d) $(Player.hh.d) $(Map.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Sprite.o : $(SRCDIR)Sprite.cc $(Sprite.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)SpriteBase.o : $(SRCDIR)SpriteBase.cc $(SpriteBase.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Boulder.o : $(SRCDIR)Boulder.cc $(Boulder.hh.d) $(Map.hh.d) $(Tile.hh.d) \
        $(MapHelpers.hh.d) $(Rect.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)MapHelpers.o : $(SRCDIR)MapHelpers.cc $(MapHelpers.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Light.o : $(SRCDIR)Light.cc $(Light.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Stat.o : $(SRCDIR)Stat.cc $(Stat.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)UIHelpers.o : $(SRCDIR)UIHelpers.cc $(UIHelpers.hh.d) $(SRCDIR)filepaths.hh $(Renderer.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Damage.o: $(SRCDIR)Damage.cc $(Damage.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Texture.o: $(SRCDIR)Texture.cc $(Texture.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)Renderer.o: $(SRCDIR)Renderer.cc $(Renderer.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

$(OBLDIR)DroppedItem.o: $(SRCDIR)DroppedItem.cc $(DroppedItem.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS)

tests : $(OBLDIR)collider_tests.o
	$(CC) $(CXXFLAGS) $^ -o $(OBLDIR)tests $(LDFLAGS) $(NOISE_FLAGS) $(LINKER_FLAGS)

$(OBLDIR)collider_tests.o : $(SRCDIR)collider_tests.cc $(Collider.hh.d)
	$(CC) $(CXXFLAGS) $^ -c $(LDFLAGS) $(LINKER_FLAGS)

clean :
	rm -f $(OBJDIR)*.o $(SRCDIR)*.gch *~ *.o *.gch

.PHONY : all clean

