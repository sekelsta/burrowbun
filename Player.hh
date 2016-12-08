#ifndef PLAYER_HH
#define PLAYER_HH

#include <vector>
#include <SDL2/SDL.h>
#include "Movable.hh"

using namespace std;


/* A class for a player. Maybe there should be a Character class it can 
    inherit from? */
class Player : public Movable {

public:
    // Constructor
    Player();
};

#endif
