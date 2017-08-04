#include <cassert>
#include <iostream>
#include "AllTheItems.hh"
#include "Player.hh"
#include "Map.hh"

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

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
        // Use use time
        player.useTimeLeft = useTime;
    }
}


// Block constructor
Block::Block(ItemType type) : Item(type) {
    /* TODO: get this info from a file instead. */
    if (type != ItemType::PICKAXE) {
        /* Set the tile type. */
        tileType = ItemMaker::itemToTile(type);
        useTime = 6;
        bonusReach = 0;
        sprite.name = "blocks.png";
        sprite.rows = 5;
        sprite.cols = 4;
        sprite.width = TILE_WIDTH;
        sprite.height = TILE_HEIGHT;
        /* Number is the number in the enum class minus the number of the first
        one. It's also the order the sprites are in on the spritesheet. */
        int number = (int)type - (int)ItemType::FIRST_BLOCK;
        sprite.row = number / sprite.cols;
        sprite.col = number % sprite.cols;
    }
}

/* Destructor must be virtual. */
Block::~Block() {};

/* Tell whether the player can reach far enough to place the block here. */
bool Block::canPlace(int x, int y, const Player &player, const Map &map) {
    // Figure out which tile the mouse is over
    int xTile = x / map.getTileWidth();
    int yTile = y / map.getTileHeight();

    // Figure out which tile the top middle of the player is at, so we can
    // see whether the tile is in range
    int xPlayer = (player.x + (player.spriteWidth / 2)) / map.getTileWidth();
    int yPlayer = (player.y + player.spriteHeight) / map.getTileHeight();

    /* And now we have our answer. */
    return player.canReach(xTile - xPlayer, yTile - yPlayer, bonusReach);
 
}

MapLayer Block::getLayer(InputType type) {
        /* Which layer to damage. */
        MapLayer layer = MapLayer::NONE;
        // If it was a left mouse button, place the tile in the foreground
        if (type == InputType::LEFT_BUTTON_PRESSED
                || type == InputType::LEFT_BUTTON_HELD) {
            layer = MapLayer::FOREGROUND;
        }
        // Otherwise, if it was the right mouse button, put the tile in the
        // background
        else if (type == InputType::RIGHT_BUTTON_PRESSED
                || type == InputType::RIGHT_BUTTON_HELD) {
            layer = MapLayer::BACKGROUND;
        }
        return layer; 
}

// When used, place the tile
void Block::use(InputType type, int x, int y, Player &player, Map &map) {
    // Only do anything if the tile is within range
    if (canPlace(x, y, player, map)) {
        // If success is still false at the end, don't set the player's use
        // time left
        bool success = false;
        MapLayer layer = getLayer(type);
        /* Only do anything if it's a real layer. */
        if (layer != MapLayer::NONE) {
            success = map.placeTile(x / map.getTileWidth(), 
                    y / map.getTileHeight(), tileType, layer);
        }
        // If success, add the use time
        player.useTimeLeft += (int)success * useTime;
    }
}

/* Pickaxe constructor. */
Pickaxe::Pickaxe(ItemType type) : Block(type) {
    /* Load the right json based on the type. */
    switch(type) {
        case ItemType::PICKAXE :
            // TODO
            break;
        default:
            assert(false);
            break;
    }

    /* Temporary, TODO: delete. */
    sprite.name = "pickaxe01.png";
    useTime = 9;
    blockDamage = 1;
    pickaxeTier = 1;
}

/* Pickaxe use. */
void Pickaxe::use(InputType type, int x, int y, Player &player, Map &map) {
    // Only do anything if the tile is within range
    if (canPlace(x, y, player, map)) {
        // If success is still false at the end, don't set the player's use
        // time left
        bool success = false;
        /* Which layer to damage. */
        MapLayer layer = getLayer(type);
        success = map.damage(x / map.getTileWidth(), y / map.getTileHeight(), 
                blockDamage, layer);
        // If success, add the use time
        player.useTimeLeft += (int)success * useTime;
    }
}

/* Turn an ItemType into the corresponding TileType. Requires that the 
tileTypes and ItemTypes are listed in the same order in their enum classes. */
TileType ItemMaker::itemToTile(ItemType itemType) {
    /* The first and last tiletypes that are also items are dirt and 
    dark brick. */
    int firstTile = (int)TileType::FIRST_ITEMED_TILE;
    int lastTile = (int)TileType::LAST_TILE;
    /* The first and last ItemTypes that are also tiles are dirt and 
    dark brick as well. */
    int firstItem = (int)ItemType::FIRST_BLOCK;
    int lastItem = (int)ItemType::LAST_BLOCK;

    assert(lastTile - firstTile == lastItem - firstItem);
    assert(firstItem <= (int)itemType);
    assert((int)itemType <= lastItem);

    /* Now just convert. */
    int answer = (int)itemType - firstItem + firstTile;
    assert(firstTile <= answer);
    assert(answer <= lastTile);
    return (TileType)answer;
}

/* Turn a TileType into the corresponding ItemType. */
ItemType ItemMaker::tileToItem(TileType tileType) {
    /* The first and last tiletypes that are also items are dirt and 
    dark brick. */
    int firstTile = (int)TileType::FIRST_ITEMED_TILE;
    int lastTile = (int)TileType::LAST_TILE;
    /* The first and last ItemTypes that are also tiles are dirt and 
    dark brick as well. */
    int firstItem = (int)ItemType::FIRST_BLOCK;
    int lastItem = (int)ItemType::LAST_BLOCK;

    assert(lastTile - firstTile == lastItem - firstItem);
    assert(firstTile <= (int)tileType);
    assert((int)tileType <= lastTile);

    /* Now just convert. */
    int answer = (int)tileType + firstItem - firstTile;
    assert(firstItem <= answer);
    assert(answer <= lastItem);
    return (ItemType)answer;
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

    // If it's a potion, make a potion
    if (isIn(potions, type)) {
        return new Potion(type);
    }
    // If it's a block, make a block
    else if ((int)ItemType::FIRST_BLOCK <= (int)type
                && (int)type <= (int)ItemType::LAST_BLOCK) {
        return new Block(type);
    }
    /* If it's a pickaxe, make a pickaxe. */
    else if (type == ItemType::PICKAXE) {
        return new Pickaxe(type);
    }
    // If it's not a subclass of item, than it's a plain old item
    else {
        return new Item(type);
    }
}

