#include <SDL2/SDL.h>
#include "Player.hh"

using namespace std;

// Constructor
Player::Player() {
    // Set the drag to not 0
    drag.x = 0.3;
    drag.y = 0.3;

    // Set the amount to accelerate by
    dAccel.x = 16;
    dAccel.y = 30;

    // Set the number of updates a jump can last
    maxJumpTime = 16;

    // Set the sprite name
    // Todo: have a race
    sprite = "humanHaha.png";
    spriteWidth = 24;
    spriteHeight = 48;
   
}

