#ifndef ACTION_HH
#define ACTION_HH

#include "Sprite.hh"

#define ITEMSTACK_FONT_SIZE 13
#define ITEMSTACK_FONT_BUFFER_Y 2
#define ITEMSTACK_FONT_BUFFER_X 3

/* Class to enumerate the different types of items. */
enum class ActionType {
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

// Forward declare
class World;

// A class to describe how the player is trying to use an item
enum class InputType {
    NONE,
    LEFT_BUTTON_PRESSED,
    RIGHT_BUTTON_PRESSED,
    LEFT_BUTTON_HELD,
    RIGHT_BUTTON_HELD
};

/* A class for storing things that can go on the hotbar (items, skills). */
class Action {
protected:
    /* Minimum time before another action can be done. */
    int useTime;

    /* Which action it is. */
    ActionType type;

public:
    // Virtual destructor
    inline virtual ~Action() {};

    /* What sprite should be displayed in the hotbar. */
    Sprite sprite;

    /* Do the action, or use the item or skill. */
    virtual void 
            use(InputType type, int x, int y, World &world) = 0;

    /* Access functions */
    inline int getUseTime() const {
        return useTime;
    }

    inline ActionType getType() const {
        return type;
    }

    static inline bool isItem(ActionType type) {
        // TODO: change when adding skills
        return true;
    }
};


#endif
