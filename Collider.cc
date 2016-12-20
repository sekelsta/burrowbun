#include <cassert>
#include "Collider.hh"

using namespace std;

// Constructor
Collider::Collider(int tileWidth, int tileHeight) : TILE_WIDTH(tileWidth),
    TILE_HEIGHT(tileHeight) {
    // Disable collisions to get a map veiwer
    enableCollisions = true;
}

// A function to maove and collide the movables
void Collider::update(const Map &map, vector<Movable *> &movables) {
    // Update the velocity of everything
    for (unsigned i = 0; i < movables.size(); i++) {
        movables[i] -> accelerate();
    }

    // Calculate the world width and height
    int worldWidth = map.getWidth() * TILE_WIDTH;
    int worldHeight = map.getHeight() * TILE_HEIGHT;

    // TODO: Move the movables and collide them

    // Move movables and stop at the edge of the map
    for (unsigned i = 0; i < movables.size(); i++) {
        movables[i] -> x += movables[i] -> getVelocity().x;
        movables[i] -> y += movables[i] -> getVelocity().y;
        // Wrap in the x direction
        movables[i] -> x += worldWidth;
        movables[i] -> x %= worldWidth;
        // Collide in the y direction
        movables[i] -> y = max(0, movables[i] -> y);
        movables[i] -> y = min(movables[i] -> y, worldHeight);
    }
}

