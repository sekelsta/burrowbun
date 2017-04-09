#ifndef ITEM_HH
#define ITEM_HH

#include "Action.hh"

/* Class to enumerate the different types of items. */
enum class ItemType {
    NONE,
    MAPLE_LEAF,
    DIRT,
    TORCH,
    POTION
};

/* The thing inventories store. */
class Item : public Action {
    // Which item it is
    ItemType type;

    // How many can be in a stack in the same slot
    int maxStack;

public:
    // Constructor
    Item(ItemType itemType);
};

#endif
