#ifndef ITEM_HH
#define ITEM_HH

#include <string>
#include "Action.hh"

/* Class to enumerate the different types of items. */
enum class ItemType {
    // NONE is just a pointer to NULL instead of an actual object
    // Types of blocks
    DIRT = 0,
    HUMUS = 1,
    CLAY = 2,
    CALCAREOUS_OOZE = 3,
    SNOW = 4,
    ICE = 5,
    STONE = 6,
    GRANITE = 7,
    BASALT = 8,
    LIMESTONE = 9,
    MUDSTONE = 10,
    PERIDOTITE = 11,
    SANDSTONE = 12,
    RED_SANDSTONE = 13,
    PLATFORM = 14,
    LUMBER = 15,
    RED_BRICK = 16,
    GRAY_BRICK = 17,
    DARK_BRICK = 18,
    SAND = 19,
    PICKAXE = 20,
    FIRST_BLOCK = DIRT,
    LAST_BLOCK = PICKAXE,
    LAST_PURE_BLOCK = SAND,

    // Other things
    MAPLE_LEAF = 21,
    HEALTH_POTION = 22,

    FIRST_ITEM = DIRT,
    LAST_ITEM = HEALTH_POTION
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
