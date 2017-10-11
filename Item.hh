#ifndef ITEM_HH
#define ITEM_HH

#include <string>
#include "Action.hh"

/* Class to enumerate the different types of items. */
enum class ItemType {
    /* NONE is just a nullptr instead of an actual object. */
    DIRT,
    TOPSOIL,
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
    MUD,
    CLOUD,
    BOULDER,
    GLACIER,
    PICKAXE,

    /* Other things. */
    MAPLE_LEAF,
    HEALTH_POTION,   

    FIRST_BLOCK = DIRT,
    LAST_BLOCK = PICKAXE,
    LAST_PURE_BLOCK = GLACIER,

    FIRST_ITEM = DIRT,
    LAST_ITEM = HEALTH_POTION
};

/* The thing inventories store. */
class Item : public Action {
    /* Which item it is. */
    ItemType itemType;

    /* How many can be in a stack in the same slot. */
    int maxStack;

public:
    // Constructor
    Item(ItemType type);

    /* Virtual use function. Does nothing. */
    virtual void use(InputType type, int x, int y, Player &player, Map &map);

    /* Destructor must be virtual. */
    virtual ~Item();

    /* Access functions. */
    inline ItemType getType() {
        return itemType;
    }

    /* Get json filename from itemtype. */
    static std::string getJsonFilename(ItemType type);
};

#endif
