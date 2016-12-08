#include <SDL2/SDL.h>
#include "Player.hh"

using namespace std;

// Constructor
Player::Player() {
    // Set the drag to not 0
    drag.x = 0.8;
    drag.y = 0.8;

    // Set the amount to accelerate by
    dAccel.x = 3;
    dAccel.y = 3;
}

