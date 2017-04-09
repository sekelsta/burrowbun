#ifndef ACTION_HH
#define ACTION_HH

#include "Sprite.hh"

/* A class for storing things that can go on the hotbar (items, skills). */
class Action {
    // Minimum time before another action can be done
    int useTime;

public:
    // What sprite should be displayed in the hotbar
    Sprite sprite;

    // Constructor
    Action();

    // What it does when used
    virtual void use();
};


#endif
