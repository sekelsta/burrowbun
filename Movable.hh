#ifndef MOVABLE_HH
#define MOVABLE_HH

#include <string>
#include "Point.hh"

// Forward declare
struct SDL_Texture;

using namespace std;

/*  A base class for anything that can move and collide on the map, 
    not including tiles. Monsters, NPCs, the player, and dropped items should
    all be subclasses. */
class Movable {
protected:
    // Fields
    string sprite;

    // Movement-related fields
    // Drag is a number between 0 and 1 which the velocity is multiplied by
    Point drag, velocity, accel, dAccel;

public:
    // Information about recent collisions
    // We need to know whether there was a collision down so that we can
    // avoid jumping midair, and we need isCollidingX to not be stopped by
    // one block high walls.
    bool isCollidingX;
    bool isCollidingDown;
    int ticksCollidingDown; // Number of updates isCollidingDown has been true
    bool isSteppingUp; // As in, stepping up one block
    int timeOffGround; // How many updates since it was on the ground
    bool collidePlatforms; // Whether or not to fall through platforms
    bool isDroppingDown; // isCollidingDown && !collidePlatforms
    // How many updates before holding down the jump key stops working. This
    // determines maximum jump height. Here -1 means infinity.
    int maxJumpTime; 

    // What number to use when calculating fall damage
    int pixelsFallen;
    // The highest point reached since the last jump, or since last touching 
    // the ground
    int maxHeight;

    // Location
    int x, y;

    // How strong is gravity (may vary by location)
    double gravity;

    // Sprite, as a texture
    SDL_Texture *texture;

    // How big is the sprite?
    int spriteWidth;
    int spriteHeight;

    // Constructor
    Movable();

    // Access functions
    Point getVelocity() const;
    void setVelocity(Point newVelocity); // Very few things should use this.
    void setAccel(Point newAccel);
    Point getDAccel() const;
    string getSprite() const;
    int getSpriteWidth() const;
    int getSpriteHeight() const;

    // Updates velocity
    void accelerate();
};

#endif
