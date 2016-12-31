#ifndef COLLIDER_HH
#define COLLIDER_HH

#include <cassert>
#include <vector>
#include "Tile.hh"
#include "Map.hh"
#include "Movable.hh"

using namespace std;

/* To be able to describe collisions better. */
enum class CollisionType {
    NONE,
    START,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    LEFT_CORNER,
    RIGHT_CORNER
};

/* A struct containing a collision type, a pointer to the relevant Tile, and
   the distance to the collision. */
struct CollisionInfo {
    Tile *tile;
    int x;
    int y;
    CollisionType type;
};

/* I want a rectangle, but I don't want to include SDL. */
struct Rect {
    int x;
    int y;
    int w;
    int h;
};

/* A class to handle collisions. It takes a map and a vector of movables
   and calculates where they are at the next update. */
class Collider {
    // Fields
    const int TILE_WIDTH;
    const int TILE_HEIGHT;

    // Whether the player collides with tiles
    // Disable to get a map veiwer
    bool enableCollisions;

    // Whether two rectangles overlap
    bool isColliding(const Rect &rectA, const Rect &rectB);

    // Given that a collision happens left or right, update info accordingly.
    void findXCollision(CollisionInfo &info, int dx, const Rect &stays);

    // Given that a collision is up or down, update info accordingly
    void findYCollision(CollisionInfo &info, int dy, const Rect &stays);

    // Returns info on the collision between a moving thing and a stationary 
    // thing. Collisions that occur even if the moving thing stays still will
    // be ignored. 
    CollisionInfo findCollision(const Rect &to, const Rect &stays, 
        int dx, int dy);

public:
    // Constructor
    Collider(int tileWidth, int tileHeight);

    // A function that takes a map and a list of things and moves them, 
    // colliding when necessary
    void update(const Map &map, vector<Movable *> &movables);
};

#endif
