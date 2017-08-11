#ifndef COLLIDER_HH
#define COLLIDER_HH

#include <iostream>
#include <cassert>
#include <vector>
#include "Tile.hh"
#include "Map.hh"
#include "Movable.hh"
#include "Rect.hh"

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
    /* The locations of the edges of the tile that could be being collided
    with, based on the direction the movable is moving, adjusted so their 
    distance from the bottom left corner of the movable is the same as,
    pre-adjustement, the distance from the edge to whichever edge of the 
    movable would collide with them had been. 
    This ends up being the movable's location plus the distance to the 
    collision in each direction. */
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
    void resolve(Tile const *tile) {
        newX = -1;
        newY = -1;
        cornerX = -1;
        switch(type) {
            case CollisionType::DOWN :
                yCoefficient *= (int)(!(tile -> getIsPlatform()));
                // Purposely no break
            case CollisionType::UP :
                newY = y;
                yCoefficient *= (int)(!(tile -> getIsSolid()));
                break;
            case CollisionType::LEFT :
            case CollisionType::RIGHT :
                newX = x;
                if (tile -> getIsSolid()) {
                    xCoefficient = 0;
                }
                break;
            case CollisionType::LEFT_CORNER :
            case CollisionType::RIGHT_CORNER :
                if (tile -> getIsSolid()) {
                    cornerX = x;
                }
                break;
            case CollisionType::NONE :
                break;
            }
            // Ignore collisions with platforms from most directions
            if (tile -> getIsPlatform() && type != CollisionType::DOWN) {
                type = CollisionType::NONE;
            }
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
    CollisionInfo findCollision(const Rect &from, const Rect &to, 
        const Rect &stays) const;

    /* Goes through the tiles near the movable, finds all collisions, and adds
    them to the vactor collisions. If any of the collisions are inevitable, it 
    returns true. Otherwise, it returns false. If dropDown is true, it will 
    not count collisions with platforms. */
    bool listCollisions(vector<CollisionInfo> &collisions, Map &map, 
        const Rect &to, const Rect &from, bool dropDown) const;

    // Takes a movable and a map, and moves it to where it should end up
    void collide(Map &map, movable::Movable &movable);

public:
    // Constructor
    Collider(int tileWidth, int tileHeight);

    // A function that takes a map and a list of things and moves them, 
    // colliding when necessary
    void update(Map &map, vector<movable::Movable *> &movables);
};

#endif
