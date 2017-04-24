#ifndef PLAYER_HH
#define PLAYER_HH

#include <vector>
#include <SDL2/SDL.h>
#include "Entity.hh"
#include "Hotbar.hh"
#include "Inventory.hh"
#include "UIHelpers.hh"

// Forward declare
class Action;

using namespace std;


/* A class for a player. Maybe there should be a Character class it can 
    inherit from? 
    If it's unclear whether something belongs in the player class or as part of
    the eventhandler, the rule of thumb is to ask whether it belongs in the
    player's savefile or in the game's config file. */
class Player : public Entity {
    // How far can they reach to place tiles
    int tileReachUp;
    int tileReachDown;
    int tileReachSideways;

public:
    /* The use time of the last item / skill used, minus the number of ticks 
    since using it. Items and skills can only be used when this is 0. */
    int useTimeLeft;

    // Where the player is being rendered on the screen
    int screenX, screenY;

    bool isInventoryOpen;
    Inventory inventory;
    Inventory trash;

    // So the mouse can move around items and put them in the hotbar
    Action *mouseSlot;

    Hotbar hotbar;

    // Constructor
    Player();

    // Switch the open / closed state of the inventory.
    void toggleInventory();

    // Whether a place is within range for placing tiles
    // x is distance from the player horizontally, in tiles.
    // y is distance from the player in tiles, with positive being above the 
    // player. bonus is the bonus range from possible unknown curcumstances
    // (e.g. this type of tile can be placed farther away, or this pickax has
    // better range).
    bool canReach(int x, int y, int bonus);

    // Whether the player can use an item / skill right now.
    bool canUse();

    // Use the item held or selected
    void useAction(InputType type, int x, int y, Map &map);
};

#endif
