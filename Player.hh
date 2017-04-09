#ifndef PLAYER_HH
#define PLAYER_HH

#include <vector>
#include <SDL2/SDL.h>
#include "Entity.hh"
#include "Hotbar.hh"
#include "Inventory.hh"
#include "UIHelpers.hh"

using namespace std;


/* A class for a player. Maybe there should be a Character class it can 
    inherit from? 
    If it's unclear whether something belongs in the player class or as part of
    the eventhandler, the rule of thumb is to ask whether it belongs in the
    player's savefile or in the game's config file. */
class Player : public Entity {

public:
    bool isInventoryOpen;
    Inventory inventory;
    Inventory trash;

    Hotbar hotbar;

    // Constructor
    Player();

    // Switch the open / closed state of the inventory.
    void toggleInventory();
};

#endif
