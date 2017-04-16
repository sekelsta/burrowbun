#include "Item.hh"

// Constructor
Item::Item(ItemType itemType) {
    type = itemType;
    sprite.name = "";
    maxStack = 1; // TODO
    // It's definately an item
    isItem = true;

    // Do stuff specific to the item type
    switch(type) {
        case ItemType::MAPLE_LEAF :
            sprite.name = "maple_leaf.png";
            sprite.width = 32;
            sprite.height = 32;
            sprite.rows = 1;
            sprite.cols = 1;
            sprite.row = 0;
            sprite.col = 0;
            break;
    }
}

// Access functions
ItemType Item::getType() {
    return type;
}

