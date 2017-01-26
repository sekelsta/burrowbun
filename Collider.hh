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

    // Function to tell whether another rectangle intersects this one.
    inline bool intersects(const Rect &that) const {
        return (x + w > that.x && x < that.x + that.w
            && y + h > that.y && y < that.y + that.h);
    }
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

    // Pretend all tiles are smaller by this amount
    int xOffset;
    int yOffset;

    // Given that a collision happens left or right, update info accordingly.
    // dx is the step size and w is the width of the player.
    void findXCollision(CollisionInfo &info, int dx, int w, const Rect &stays);

    // Given that a collision is up or down, update info accordingly
    // dy is the step size and h is the height of the player.
    void findYCollision(CollisionInfo &info, int dy, int h, const Rect &stays);

    // Returns info on the collision between a moving thing and a stationary 
    // thing. Collisions that occur even if the moving thing stays still will
    // be ignored. 
    CollisionInfo findCollision(const Rect &to, const Rect &stays, 
        int dx, int dy);

    // Takes a movable and a map, and moves it to where it should end up
    void collide(const Map &map, Movable &movable);

public:
    // Constructor
    Collider(int tileWidth, int tileHeight);

    // A function that takes a map and a list of things and moves them, 
    // colliding when necessary
    void update(const Map &map, vector<Movable *> &movables);
};

#endif
