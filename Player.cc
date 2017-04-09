#include <SDL2/SDL.h>
#include "Player.hh"
#include <iostream>

using namespace std;

// Constructor
Player::Player() : inventory(10, 6), trash(1, 1) {
    // Set the drag to not 0
    drag.x = 0.3;
    drag.y = 11.0 / 12.0;

    // Set the amount to accelerate by
    dAccel.x = 10;
    dAccel.y = 3;

    // Set the number of updates a jump can last
    maxJumpTime = 8;

    // Set the maximum distance it can fall before taking damage (-1 for
    // infinity)
    maxFallDistance = 300;

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

    // Set the location of the inventory
    inventory.x = hotbar.xStart;
    // The position of the bottom of the hotbar
    inventory.y = hotbar.yStart + hotbar.frame.height + hotbar.offsetDown;
    // The size of the gap between the hotbar and the inventory
    inventory.y += 16;
    trash.x = inventory.x;
    // The 32s here are for Inventory::squareSprite.width and height,
    // which haven't been initialized yet. TODO: fix
    trash.x += (inventory.getWidth() - 1) * 32;
    trash.y = inventory.y + 4;
    trash.y += inventory.getHeight() * 32;

    // Have them update where their clickboxes are
    inventory.updateClickBoxes();
    trash.updateClickBoxes();

    isInventoryOpen = false;
}

// Switch whether the inventory is open or closed
void Player::toggleInventory() {
    isInventoryOpen = !isInventoryOpen;
}

