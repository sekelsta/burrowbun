#ifndef POTION_HH
#define POTION_HH

#include "Item.hh"
#include "../world/World.hh"

// All the child classes of "Item"
// Items that change the player's stats
class Potion : public Item {
    int healthGained;
    int fullnessGained;
    int manaGained;
    int woundsCured; // Removes the completely empty part of health
    int hungerCured; // Removes the completely empty part of fullness
    int manaCured; // Removed the completely empty part of mana

public:
    // Constructor
    Potion(ActionType type);

    // What to do when used
    virtual bool use_internal(InputType type, int x, int y, World &world);
};

#endif
