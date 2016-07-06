#ifndef COLLIDER_HH
#define COLLIDER_HH

#include <cassert>
#include <vector>
#include "Tile.hh"
#include "Map.hh"
#include "Movable.hh"

using namespace std;

/* A class to handle collisions. It takes a map and a vector of movables
   and claculates where they are at the next update. */
class Collider {
    // Fields
    const int TILE_WIDTH;
    const int TILE_HEIGHT;

public:
    // Constructor
    Collider(int tileWidth, int tileHeight);

    // A function that takes a map and a list of things and moves them, 
    // colliding when necessary
    void update(const Map &map, vector<Movable *> &movables);
};

#endif
