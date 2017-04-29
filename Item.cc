#include <vector>
#include "Item.hh"

// Constructor
Item::Item(ItemType type) {
    itemType = type;
    sprite.name = "";
    maxStack = 1; // TODO
    // It's definately an item
    isItem = true;

    sprite.width = 32;
    sprite.height = 32;

    // Default use time for blocks
    int blockTime = 6;

    // Do stuff specific to the item type
    switch(type) {
        case ItemType::MAPLE_LEAF :
            sprite.name = "maple_leaf.png";
            sprite.rows = 1;
            sprite.cols = 1;
            sprite.row = 0;
            sprite.col = 0;
            useTime = 1;
            break;
        case ItemType::HEALTH_POTION :
            sprite.name = "potions.png";
            sprite.rows = 8;
            sprite.cols = 8;
            sprite.row = 4;
            sprite.col = 1;
            useTime = 32;
            break;
    }
}

// Destructor must be virtual
Item::~Item() {};

// Access functions
ItemType Item::getType() {
    return itemType;
}

