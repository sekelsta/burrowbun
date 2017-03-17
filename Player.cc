#include <SDL2/SDL.h>
#include "Player.hh"

using namespace std;

// Constructor
Player::Player() {
    // Set the drag to not 0
    drag.x = 0.3;
    drag.y = 0.5;

    // Set the amount to accelerate by
    dAccel.x = 10;
    dAccel.y = 24;

    // Set the number of updates a jump can last
    maxJumpTime = 8;

    // Set the sprite name
    // Todo: have a race
    sprite = "bunny.png";
    spriteWidth = 38;
    spriteHeight = 32;
   
}

