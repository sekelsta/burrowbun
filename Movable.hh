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

    // How big is the sprite?
    int spriteWidth;
    int spriteHeight;

    // Movement-related fields
    // Drag is a number between 0 and 1 which the velocity is multiplied by
    Point drag, velocity, accel, dAccel;

public:
    // Location
    int x, y;

    // Sprite, as a texture
    SDL_Texture *texture;

    // Constructor
    Movable();

    // Access functions
    Point getVelocity();
    void setAccel(Point newAccel);
    Point getDAccel();
    string getSprite();
    int getSpriteWidth();
    int getSpriteHeight();

    // Updates velocity
    void accelerate();
};

#endif
