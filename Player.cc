#include <SDL2/SDL.h>
#include "Player.hh"

using namespace std;

// Constructor
Player::Player() {
    // Set the max velocity to not 0
    xMaxSpeed = 50;
    yMaxSpeed = 50;

    // There might be a less repetitive way to do this.
    keySettings.leftKeys.push_back(SDLK_LEFT);
    keySettings.leftKeys.push_back(SDLK_a);
    keySettings.rightKeys.push_back(SDLK_RIGHT);
    keySettings.rightKeys.push_back(SDLK_d);
    keySettings.upKeys.push_back(SDLK_UP);
    keySettings.upKeys.push_back(SDLK_w);
    keySettings.downKeys.push_back(SDLK_DOWN);
    keySettings.downKeys.push_back(SDLK_s);
}

// Access functions
KeySettings Player::getKeySettings() {
    return keySettings;
}

void Player::setKeySettings(KeySettings &newSettings) {
    keySettings = newSettings;
}
