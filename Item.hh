#ifndef ITEM_HH
#define ITEM_HH

#include "Action.hh"

/* Class to enumerate the different types of items. */
enum class ItemType {
    // NONE is just a pointer to NULL instead of an actual object
    MAPLE_LEAF,
    DIRT,
    HEALTH_POTION
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

    // Access functions
    ItemType getType();
};

#endif
