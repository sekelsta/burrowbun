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

    // Initialize stats
    health.max = 100;
    health.totalWidth = 190;
    health.h = 8;
    health.fill();
    hunger.max = 100;
    hunger.totalWidth = 190;
    hunger.h = 8;
    hunger.fill();
    mana.max = 100; 
    mana.totalWidth = 190;
    mana.h = 8;
    mana.fill();  

    // Set the colors
    health.fullColor.r = 0xFF;
    health.fullColor.g = 0x00;
    health.fullColor.b = 0x00;
    health.partColor.r = 0x60;
    health.partColor.g = 0x00;
    health.partColor.b = 0x00;
    health.emptyColor.r = 0x00;
    health.emptyColor.g = 0x00;
    health.emptyColor.b = 0x00;
    hunger.fullColor.r = 0xF0;
    hunger.fullColor.g = 0xF0;
    hunger.fullColor.b = 0x00;
    hunger.partColor.r = 0x88;
    hunger.partColor.g = 0x44;
    hunger.partColor.b = 0x00;
    hunger.emptyColor = health.emptyColor; // black
    mana.fullColor.r = 0x10;
    mana.fullColor.g = 0x28;
    mana.fullColor.b = 0xFF;
    mana.partColor.r = 0x08;
    mana.partColor.g = 0x18;
    mana.partColor.b = 0x60;
    mana.emptyColor = health.emptyColor; // still black
}

