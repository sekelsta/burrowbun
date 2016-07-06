#include <SDL2/SDL.h>
#include "Player.hh"

using namespace std;

// Constructor
Player::Player() {
    // Set the max velocity to not 0
    xMaxSpeed = 5;
    yMaxSpeed = 5;

    leftKeys.push_back(SDLK_LEFT);
    leftKeys.push_back(SDLK_a);
    rightKeys.push_back(SDLK_RIGHT);
    rightKeys.push_back(SDLK_d);
    upKeys.push_back(SDLK_UP);
    upKeys.push_back(SDLK_w);
    downKeys.push_back(SDLK_DOWN);
    downKeys.push_back(SDLK_s);
}

// Access functions - this is so ugly!
vector<SDL_Keycode> Player::getLeftKeys() {
    return leftKeys;
}

vector<SDL_Keycode> Player::getRightKeys() {
    return rightKeys;
}

vector<SDL_Keycode> Player::getUpKeys() {
    return upKeys;
}

vector<SDL_Keycode> Player::getDownKeys() {
    return downKeys;
}
