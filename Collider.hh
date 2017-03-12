#ifndef COLLIDER_HH
#define COLLIDER_HH

#include <iostream>
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
    // Whether other collisions can prevent this one
    bool isInevitable;
    // For resolving the collision
    int xCoefficient;
    int yCoefficient;
    int newX;
    int newY;
    int cornerX;

    // Resolve a collision
    // Technically it says the pointer to the Tile is const, but the Tile 
    // should be const as well. It just shouldn't be a pointer to a const tile.
    void resolve(const Tile *tile) {
        newX = -1;
        newY = -1;
        cornerX = -1;
        switch(type) {
            case CollisionType::DOWN :
                yCoefficient *= (int)(!(tile -> isPlatform));
                // Purposely no break
            case CollisionType::UP :
                newY = y;
                yCoefficient *= (int)(!(tile -> isSolid));
                break;
            case CollisionType::LEFT :
            case CollisionType::RIGHT :
                newX = x;
                if (tile -> isSolid) {
                    xCoefficient = 0;
                }
                break;
            case CollisionType::LEFT_CORNER :
            case CollisionType::RIGHT_CORNER :
                if (tile -> isSolid) {
                    cornerX = x;
                }
                break;
            case CollisionType::NONE :
                break;
            }
        }
    
};

/* Rectangle, capable of seeing if another intersects it taking into account
    the world wrapping around the x direction. */
class Rect {
public:
    int worldWidth;
    int x;
    int y;
    int w;
    int h;

    // Function to tell whether another rectangle intersects this one.
    inline bool intersects(const Rect &that) const {
        bool intersectsX = x + w > that.x && x < that.x + that.w;
        /* If either rectangle wraps but not both, we should also see if they
           intersect when  one is moved to the other side of the line. */
        if ((x + w < worldWidth) != (that.x + that.w < worldWidth)) {
            intersectsX = intersectsX || (x + w + worldWidth > that.x 
                && x + worldWidth < that.x + that.w) 
                || (x + w > that.x + worldWidth
                && x < that.x + that.w + worldWidth);
        }
        

        return (intersectsX && y + h > that.y && y < that.y + that.h);
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
    void findXCollision(CollisionInfo &info, int dx, int w, const Rect &stays)
        const;

    // Given that a collision is up or down, update info accordingly
    // dy is the step size and h is the height of the player.
    void findYCollision(CollisionInfo &info, int dy, int h, const Rect &stays)
        const;

    // Returns info on the collision between a moving thing and a stationary 
    // thing. Collisions that occur even if the moving thing stays still will
    // be ignored. 
    CollisionInfo findCollision(const Rect &to, const Rect &stays, 
        int dx, int dy) const;

    /* Goes through the tiles near the movable, finds all collisions, and adds
    them to the vactor collisions. If any of the collisions are inevitable, it 
    returns true. Otherwise, it returns false. */
    bool listCollisions(vector<CollisionInfo> &collisions, const Map &map, 
        const Rect &to, const Rect &from) const;

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
