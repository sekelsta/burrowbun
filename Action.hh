#ifndef ACTION_HH
#define ACTION_HH

#include "Sprite.hh"

// Forward declare
class Player;
class Map;

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
    // Minimum time before another action can be done
    int useTime;

public:
    // What sprite should be displayed in the hotbar
    Sprite sprite;

    // Whether it's an item
    bool isItem;

    // Constructor
    Action();

    // What it does when used
    virtual void use(InputType type, int x, int y, Player &player, Map &map);
};


#endif
