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

/* Rectangle, capable of seeing if another intersects it taking into account
    the world wrapping around the x direction. */
class Rect {
public:
    int worldWidth;
    int x;
    int y;
    int w;
    int h;

    /* Function to tell whether another rectangle intersects this one.
    Note that it may be the case that one rectangle or other has x < 0
    or x > worldWidth. */
    inline bool intersects(const Rect &that) const {
        /* Get x values within the correct range. */
        int thisX = (x + worldWidth) % worldWidth;
        int thatX = (that.x + worldWidth) % worldWidth;
        assert(0 <= thisX);
        assert(0 <= y);
        assert(0 <= w);
        assert(0 <= thatX);
        assert(0 <= that.y);
        assert(0 <= that.w);
        assert(x < worldWidth);
        assert(thatX < worldWidth);

        bool intersectsX = (thisX + w > thatX) && (thisX < thatX + that.w);
        /* If either rectangle wraps but not both, we should also see if they
           intersect when one is moved to the other side of the line. */
        if ((thisX + w < worldWidth) != (thatX + that.w < worldWidth)) {
            bool thisWraps = (thatX + worldWidth < thisX + w)
                    && (thisX < thatX + that.w + worldWidth);
            bool thatWraps = (thatX < thisX + w + worldWidth) 
                    && (thisX + worldWidth < thatX + that.w);
            if ((y + h > that.y) && (y < that.y + that.h) && !intersectsX
                    && (thisX > worldWidth - 20 || thatX > worldWidth - 20)
                    && (!thisWraps && !thatWraps)) {
            std::cout << "A rectangle wrapped." << std::endl;
            std::cout << "thisX, this.y, this.w, this.h = " << thisX << ", ";
            std::cout << y << ", " << w << ", " << h << std::endl;
            std::cout << "thatX, that.y, that.w, that.h = " << thatX << ", ";
            std::cout << that.y << ", " << that.w << ", " << that.h << "\n";
            std::cout << "this wrapping: " << thisWraps << std::endl;
            std::cout << "that wrapping: " << thatWraps << std::endl;
            }
            intersectsX = intersectsX || thisWraps || thatWraps;
        }
        

        return (intersectsX && (y + h > that.y) && (y < that.y + that.h));
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
    returns true. Otherwise, it returns false. If dropDown is true, it will 
    not count collisions with platforms. */
    bool listCollisions(vector<CollisionInfo> &collisions, Map &map, 
        const Rect &to, const Rect &from, bool dropDown) const;

    // Takes a movable and a map, and moves it to where it should end up
    void collide(Map &map, Movable &movable);

public:
    // Constructor
    Collider(int tileWidth, int tileHeight);

    // A function that takes a map and a list of things and moves them, 
    // colliding when necessary
    void update(Map &map, vector<Movable *> &movables);
};

#endif
