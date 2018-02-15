#ifndef WORLD_HH
#define WORLD_HH

#include "Map.hh"
#include "Collider.hh"
#include "DroppedItem.hh"
#include "Movable.hh"
#include "Player.hh"
#include <vector>

class World {
public:
    Map map;
    Player player;

    /* Vectors to hold all the things that need to collide. */
    std::vector<DroppedItem *> droppedItems;
    std::vector<movable::Movable *> movables;

private:
    Collider collider;

    /* How many ticks since the map was loaded. */
    unsigned int tick;

public:
    World(std::string filename, int tileWidth, int tileHeight, std::string path);

    void update();
};

#endif
