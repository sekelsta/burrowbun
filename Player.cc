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

    // Set the maximum distance it can fall before taking damage (-1 for
    // infinity)
    maxFallDistance = 200;

    // Set the sprite name
    // Todo: have a race
    sprite = "bunny.png";
    spriteWidth = 38;
    spriteHeight = 32;

    // Initialize stats
    health.maxStat = 100;
    health.totalWidth = 190;
    health.h = 8;
    health.fill();
    hunger.maxStat = 100;
    hunger.totalWidth = 190;
    hunger.h = 8;
    hunger.fill();
    mana.maxStat = 100; 
    mana.totalWidth = 190;
    mana.h = 8;
    mana.fill();  

    // We don't need to set the statbar colors because the entity constructor
    // already did.
}

