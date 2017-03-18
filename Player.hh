#ifndef PLAYER_HH
#define PLAYER_HH

#include <vector>
#include <SDL2/SDL.h>
#include "Movable.hh"
#include "Hotbar.hh"

using namespace std;


/* A class for a player. Maybe there should be a Character class it can 
    inherit from? 
    If it's unclear whether something belongs in the player class or as part of
    the eventhandler, the rule of thumb is to ask whether it belongs in the
    player's savefile or in the game's config file. */
class Player : public Movable {

public:
    Hotbar hotbar;

    // Constructor
    Player();
};

#endif
