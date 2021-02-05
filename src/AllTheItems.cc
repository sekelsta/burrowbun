#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
#include "AllTheItems.hh"
#include "Player.hh"
#include "Map.hh"
#include "World.hh"
#include "json.hh"

using json = nlohmann::json;
using namespace std;

// Potion constructor
Potion::Potion(ActionType type, string path) : Item(type, path) {
    /* Figure out which json file to use. */
    std::string filename = path + Item::getJsonFilename(type);

    /* Put the data into the json. */
    std::ifstream infile(filename);
    /* Check that the file could be opened. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);

    /* Set values equal to the json's values. */
    healthGained = j["healthGained"];
    fullnessGained = j["fullnessGained"];
    manaGained = j["manaGained"];
    woundsCured = j["woundsCured"];
    hungerCured = j["hungerCured"];
    manaCured = j["manaCured"];
}

// Add the potion amount to all the stats
bool Potion::use_internal(InputType type, int x, int y, World &world) {
    // Add the potion amount to the player, but only if the left mouse
    // button wqs pressed (not held, and not the right button).
    if (type == InputType::LEFT_BUTTON_PRESSED) {
        world.player.health.addPart(woundsCured);
        world.player.fullness.addPart(hungerCured);
        world.player.mana.addPart(manaCured);
        world.player.health.addFull(healthGained);
        world.player.fullness.addFull(fullnessGained);
        world.player.mana.addFull(manaGained);
        return true;
    }
    return false;
}


// Block constructor
Block::Block(ActionType type, string path) : Item(type, path) {
    /* Make sure we should actually be a block. */
    assert(ActionType::FIRST_BLOCK <= type);
    assert(type <= ActionType::LAST_BLOCK);

    /* Read in the json. */
    std::string filename = path + Item::getJsonFilename(type);
    std::ifstream infile(filename);
    /* Check that the file could be opened. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);

    /* Set values. */
    bonusReach = j["bonusReach"];
    
    /* (The json doesn't include the tiletype) */
    if (type <= ActionType::LAST_PURE_BLOCK) {
        tileType = ItemMaker::itemToTile(type);
    }
    else {
        tileType = TileType::EMPTY;
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
    int xPlayer = (player.getRect().x + (player.getWidth() / 2));
    xPlayer /= map.getTileWidth();
    int yPlayer = (player.getRect().y + player.getHeight());
    yPlayer /= map.getTileHeight();

    /* And now we have our answer. We don't need to do anything special about 
    wrapping the map because xTile will already be outside of the map range if 
    that's needed to get it numerically closer to the player. */
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
bool Block::use_internal(InputType type, int x, int y, World &world) {
    // Only do anything if the tile is within range
    if (!canPlace(x, y, world.player, world.map)) {
        return false;
    }

    MapLayer layer = getLayer(type);

    /* Only do anything if it's a real layer. */
    if (layer == MapLayer::NONE) {
        return false;
    }

    /* If success is still false at the end, don't set the player's use
    time left. */
    bool success = world.map.placeTile(
            world.map.getMapCoords(x, y, layer), tileType);

    return success;
}

/* Pickaxe constructor. */
Pickaxe::Pickaxe(ActionType type, string path) : Block(type, path) {
    /* Load the right json based on the type. */
    std::string filename = path + Item::getJsonFilename(type);
    std::ifstream infile(filename);
    /* Check that the file could be opened. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);

    blockDamage = j["blockDamage"];
    tier = j["tier"];
}

/* Pickaxe use. */
bool Pickaxe::use_internal(InputType type, int x, int y, World &world) {
    // Only do anything if the tile is within range
    if (!canPlace(x, y, world.player, world.map)) {
        return false;
    }

    /* Which layer to damage. */
    MapLayer layer = getLayer(type);
    Location place = world.map.getMapCoords(x, y, layer);
    /* Only mine blocks this pickaxe is capable of mining. */
    if (world.map.getTile(place) -> getTier() > tier) {
        return false;
    }

    bool success = world.map.damage(place, blockDamage, world.droppedItems);
    return success;
}

/* Turn an ActionType into the corresponding TileType. Requires that the 
tileTypes and ActionTypes are listed in the same order in their enum classes. */
TileType ItemMaker::itemToTile(ActionType type) {
    /* The first and last tiletypes that are also items are dirt and 
    dark brick. */
    int firstTile = (int)TileType::FIRST_ITEMED_TILE;
    int lastTile = (int)TileType::LAST_TILE;
    /* The first and last ActionTypes that are also tiles are dirt and 
    dark brick as well. */
    int firstItem = (int)ActionType::FIRST_BLOCK;
    int lastItem = (int)ActionType::LAST_PURE_BLOCK;

    assert(lastTile - firstTile == lastItem - firstItem);
    assert(firstItem <= (int)type);
    assert((int)type <= lastItem);

    /* Now just convert. */
    int answer = (int)type - firstItem + firstTile;
    assert(firstTile <= answer);
    assert(answer <= lastTile);
    return (TileType)answer;
}

/* Turn a TileType into the corresponding ActionType. */
ActionType ItemMaker::tileToItem(TileType tileType) {
    /* The first and last tiletypes that are also items are dirt and 
    dark brick. */
    int firstTile = (int)TileType::FIRST_ITEMED_TILE;
    int lastTile = (int)TileType::LAST_TILE;
    /* The first and last ActionTypes that are also tiles are dirt and 
    dark brick as well. */
    int firstItem = (int)ActionType::FIRST_BLOCK;
    int lastItem = (int)ActionType::LAST_PURE_BLOCK;

    assert(lastTile - firstTile == lastItem - firstItem);
    assert(firstTile <= (int)tileType);
    assert((int)tileType <= lastTile);

    /* Now just convert. */
    int answer = (int)tileType + firstItem - firstTile;
    assert(firstItem <= answer);
    assert(answer <= lastItem);
    return (ActionType)answer;
}

// Whether the type is in the vector
bool ItemMaker::isIn(std::vector<ActionType> items, ActionType type) {
    for (unsigned int i = 0; i < items.size(); i++) {
        if (items[i] == type) {
            return true;
        }
    }
    // We've gone through the whole thing and it's not in there
    return false;
}

// Take an item type and make the correct child class based on that
Item *ItemMaker::makeItem(ActionType type, string path) {
    // A list of all the item types that should be potions
    std::vector<ActionType> potions;
    potions.push_back(ActionType::HEALTH_POTION);

    // If it's a potion, make a potion
    if (isIn(potions, type)) {
        return new Potion(type, path);
    }
    // If it's a block, make a block
    else if ((int)ActionType::FIRST_BLOCK <= (int)type
                && (int)type <= (int)ActionType::LAST_PURE_BLOCK) {
        return new Block(type, path);
    }
    /* If it's a pickaxe, make a pickaxe. */
    else if (type == ActionType::PICKAXE) {
        return new Pickaxe(type, path);
    }
    // If it's not a subclass of item, than it's a plain old item
    else {
        return new Item(type, path);
    }
}

