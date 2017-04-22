#include <cassert>
#include <iostream>
#include "AllTheItems.hh"
#include "Player.hh"
#include "Map.hh"

// Potion constructor
Potion::Potion(ItemType type) : Item(type) {
    // Set values to defaults
    healthGained = 0;
    staminaGained = 0;
    manaGained = 0;
    woundsCured = 0;
    hungerCured = 0;
    manaCured = 0;
 
    // Changed some values based on what type of potion
    switch (type) {
        case ItemType::HEALTH_POTION :
            healthGained = 50;
            break;
        default :
            // We forgot a case, or something's trying to make a potion out of 
            // something that's not a potion
            assert(false);
            break;
    }
}

// Add the potion amount to all the stats
void Potion::use(InputType type, int x, int y, Player &player, Map &map) {
    // Add the potion amount to the player, but only if the left mouse
    // button wqs pressed (not held, and not the right button).
    if (type == InputType::LEFT_BUTTON_PRESSED) {
        player.health.addPart(woundsCured);
        player.stamina.addPart(hungerCured);
        player.mana.addPart(manaCured);
        player.health.addFull(healthGained);
        player.stamina.addFull(staminaGained);
        player.mana.addFull(manaGained);
        // TODO: make consumable
    }
}


// Block constructor
Block::Block(ItemType type) : Item(type) {
    switch (type) {
        case ItemType::DIRT :
            tileType = TileType::DIRT;
            break;
        default :
            // Either we forgot a case, or something tried to make a block
            // out of something that's not a block
            assert(false);
            break;
    }
}

// When used, place the tile
void Block::use(InputType type, int x, int y, Player &player, Map &map) {
    // Figure out which tile the mouse is over
    int xTile = x / map.getTileWidth();
    int yTile = y / map.getTileHeight();

    // Figure out which tile the top middle of the player is at, so we can
    // see whether the tile is in range
    int xPlayer = (player.x + (player.spriteWidth / 2)) / map.getTileWidth();
    int yPlayer = (player.y + player.spriteHeight) / map.getTileHeight();

    // Only do anything if the tile is within range
    if (player.canReach(xTile - xPlayer, yTile - yPlayer, 0)) {
        // If it was a left mouse button, place the tile in the foreground
        if (type == InputType::LEFT_BUTTON_PRESSED
                || type == InputType::LEFT_BUTTON_HELD) {
            map.placeForeground(xTile, yTile, tileType);
        }
        // Otherwise, if it was the right mouse button, put the tile in the
        // background
        if (type == InputType::RIGHT_BUTTON_PRESSED
                || type == InputType::RIGHT_BUTTON_HELD) {
            map.placeBackground(xTile, yTile, tileType);
        }
    }
}

// Whether the type is in the vector
bool ItemMaker::isIn(std::vector<ItemType> items, ItemType type) {
    for (unsigned int i = 0; i < items.size(); i++) {
        if (items[i] == type) {
            return true;
        }
    }
    // We've gone through the whole thing and it's not in there
    return false;
}

// Take an item type and make the correct child class based on that
Item *ItemMaker::makeItem(ItemType type) {
    // A list of all the item types that should be potions
    std::vector<ItemType> potions;
    potions.push_back(ItemType::HEALTH_POTION);

    // A list of all the item types that should be blocks
    std::vector<ItemType> blocks;
    blocks.push_back(ItemType::DIRT);

    // If it's a potion, make a potion
    if (isIn(potions, type)) {
        return new Potion(type);
    }
    // If it's a block, make a block
    else if (isIn(blocks, type)) {
        return new Block(type);
    }
    // If it's not a subclass of item, than it's a plain old item
    else {
        return new Item(type);
    }
}

