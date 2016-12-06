#ifndef PLAYER_HH
#define PLAYER_HH

#include <vector>
#include <SDL2/SDL.h>
#include "Movable.hh"

using namespace std;

/* A struct to hold information about which keys do what. This is so that
   later the player can change these settings. */
struct KeySettings {
    // Which keys are for movement
    vector<SDL_Keycode> leftKeys, rightKeys, upKeys, downKeys;
};

/* A class for a player. Maybe there should be a Character class it can 
    inherit from? */
class Player : public Movable {
    // Which keys do what
    KeySettings keySettings;

public:
    // Constructor
    Player();

    // Access methods
    KeySettings getKeySettings();
    void setKeySettings(KeySettings &newSettings); // non-ideal name
};

#endif
