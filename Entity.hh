#ifndef ENTITY_HH
#define ENTITY_HH

#include "Movable.hh"
#include "UIHelpers.hh"

/* A class for monsters, NPCs, and players to inherit from.
If any features are added later that should reset fall damage, they should
do the resetting of the fall damage in this class or one of its children. */
class Entity : public Movable {

public:
    // To hold information on the stats
    StatBar health;
    StatBar fullness; // Stamina being how much hunger it doesn't have
    StatBar mana;

    // Farthest it can fall before taking fall damage (in pixels)
    int maxFallDistance;

    // Constructor
    Entity(); 

    // Virtual destructor
    virtual ~Entity();

    virtual void takeDamage(int normal, int wounds);

    // Calculate fall damage
    virtual void takeFallDamage();
};

#endif
