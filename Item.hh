#ifndef ITEM_HH
#define ITEM_HH

#include <string>
#include "Action.hh"

/* Class to enumerate the different types of items. */
enum class ItemType {
    // NONE is just a pointer to NULL instead of an actual object
    // Types of blocks
    DIRT,
    HUMUS,
    CLAY,
    CALCAREOUS_OOZE,
    SNOW,
    ICE,
    STONE,
    GRANITE,
    BASALT,
    LIMESTONE,
    MUDSTONE,
    PERIDOTITE,
    SANDSTONE,
    RED_SANDSTONE,
    PLATFORM,
    LUMBER,
    RED_BRICK,
    GRAY_BRICK,
    DARK_BRICK,
    SAND,
    FIRST_BLOCK = DIRT,
    LAST_BLOCK = SAND,

    // Other things
    MAPLE_LEAF,
    HEALTH_POTION,
    PICKAXE,

    FIRST_ITEM = DIRT,
    LAST_ITEM = PICKAXE
};

/* The thing inventories store. */
class Item : public Action {
    // Which item it is
    ItemType itemType;

    // How many can be in a stack in the same slot
    int maxStack;

public:
    // Constructor
    Item(ItemType type);

    // Destructor must be virtual
    virtual ~Item();

    // Access functions
    ItemType getType();

    /* Get json filename from itemtype. */
    static std::string getJsonFilename(ItemType type);
};

#endif
