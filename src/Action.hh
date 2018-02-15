#ifndef ACTION_HH
#define ACTION_HH

#include "Sprite.hh"

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
    // Minimum time before another action can be done
    int useTime;

public:
    // Virtual destructor
    inline virtual ~Action() {};

    // What sprite should be displayed in the hotbar
    Sprite sprite;

    // Whether it's an item
    bool isItem;

    /* Do the action, or use the item or skill. */
    virtual void 
            use(InputType type, int x, int y, World &world) = 0;

    // Access function
    inline int getUseTime() const {
        return useTime;
    }
};


#endif
