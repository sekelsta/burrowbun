#include "ItemMaker.hh"
#include "Potion.hh"
#include "Block.hh"
#include "Pickaxe.hh"

#include <cassert>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

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
Item *ItemMaker::makeItem(ActionType type) {
    // A list of all the item types that should be potions
    std::vector<ActionType> potions;
    potions.push_back(ActionType::HEALTH_POTION);
    potions.push_back(ActionType::DANDELION);

    // If it's a potion, make a potion
    if (isIn(potions, type)) {
        return new Potion(type);
    }
    // If it's a block, make a block
    else if ((int)ActionType::FIRST_BLOCK <= (int)type
                && (int)type <= (int)ActionType::LAST_PURE_BLOCK) {
        return new Block(type);
    }
    /* If it's a pickaxe, make a pickaxe. */
    else if (type == ActionType::PICKAXE) {
        return new Pickaxe(type);
    }
    // If it's not a subclass of item, than it's a plain old item
    else {
        return new Item(type);
    }
}

