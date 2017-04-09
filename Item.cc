#include "Item.hh"

// Constructor
Item::Item(ItemType itemType) {
    type = itemType;
    sprite.name = "";
    maxStack = 1; // TODO

    // Do stuff specific to the item type
    switch(type) {
        case ItemType::MAPLE_LEAF :
            sprite.name = "maple_leaf.png";
            break;
    }
}



