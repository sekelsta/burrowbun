#include <SDL2/SDL.h>
#include "Player.hh"

using namespace std;

// Constructor
Player::Player() {
    // Set the drag to not 0
    drag.x = 0.3;
    drag.y = 0.3;

    // Set the amount to accelerate by
    dAccel.x = 16;
    dAccel.y = 16;
}

