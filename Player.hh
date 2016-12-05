#ifndef PLAYER_HH
#define PLAYER_HH

#include <vector>
#include <SDL2/SDL.h>
#include "Movable.hh"

using namespace std;

/* A class for a player. Maybe there should be a Character class it can 
    inherit from? */
class Player : public Movable {
    // Which keys are for movement
    vector<SDL_Keycode> leftKeys, rightKeys, upKeys, downKeys;

public:
    // Constructor
    Player();

    // Access methods
    // TODO: It would probably be better if I friended EventHandler or some 
    // function of it.
    vector<SDL_Keycode> getLeftKeys();
    vector<SDL_Keycode> getRightKeys();
    vector<SDL_Keycode> getUpKeys();
    vector<SDL_Keycode> getDownKeys();
};

#endif
