#include <cassert>
#include <iostream>
#include "EventHandler.hh"

using namespace std;

// Tell whether a scancode is in a vector
bool EventHandler::isIn(SDL_Scancode key, vector<SDL_Scancode> keys) {
    for (unsigned i = 0; i < keys.size(); i++) {
        if (keys[i] == key) {
            return true;
        }
    }

    // None of the keys in the vector match the key
    return false;
}

// Tell whether a vector has a key that's being held down
bool EventHandler::isHeld(const Uint8 *state, vector<SDL_Scancode> keys) {
    for (unsigned int i = 0; i < keys.size(); i++) {
        if (state[keys[i]]) {
            return true;
        }
    }

    return false;
}


// Public methods

// Constructor
EventHandler::EventHandler() {
    // Assume the player starts off not moving
    left = false;
    right = false;
    up = false;
    down = false;

    move = 0;

    // There might be a less repetitive way to do this.
    keySettings.leftKeys.push_back(SDL_SCANCODE_LEFT);
    keySettings.leftKeys.push_back(SDL_SCANCODE_A);
    keySettings.rightKeys.push_back(SDL_SCANCODE_RIGHT);
    keySettings.rightKeys.push_back(SDL_SCANCODE_D);
    keySettings.upKeys.push_back(SDL_SCANCODE_UP);
    keySettings.upKeys.push_back(SDL_SCANCODE_W);
    keySettings.downKeys.push_back(SDL_SCANCODE_DOWN);
    keySettings.downKeys.push_back(SDL_SCANCODE_S);
}

// Access functions
KeySettings EventHandler::getKeySettings() {
    return keySettings;
}

void EventHandler::setKeySettings(KeySettings &newSettings) {
    keySettings = newSettings;
}

// Handle window events
void EventHandler::windowEvent(const SDL_Event &event, bool &isFocused,
                                    WindowHandler &window) {
    switch(event.window.event) {
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            isFocused = true;
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            isFocused = false;
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            window.setMinimized(true);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            window.setMinimized(false);
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            window.setMinimized(false);
            // Purposely no break
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            window.resize(event.window.data1, event.window.data2);
            break;
        default:
            // cerr << "Recieved unsupported window event." << endl;
            break;
    }
}

// Do whatever should be done when a mouse event happens
void EventHandler::mouseEvent(const SDL_Event &event) {
    // TODO
}

// Do whatever should be done when key presses or releases happen
void EventHandler::keyEvent(const SDL_Event &event) {
    SDL_Scancode key = event.key.keysym.scancode;

    // Here we should handle keys which don't need to be held down to work.
    if (event.type == SDL_KEYUP) {
        // Pass
    }
    else if (key == SDL_SCANCODE_J) {
        // Move one pixel down
        move = -1;
    }
    else if (key == SDL_SCANCODE_K) {
        // Move one pixel up
        move = 1;
    }
    else {
        move = 0;
    }
}

// Do stuff that depends on keys being held down.
void EventHandler::updateKeys(const Uint8 *state) {
    // Initialize
    left = false;
    right = false;
    up = false;
    down = false;

    // Try to tell whether keys that matter are up or down
    if (isHeld(state, keySettings.leftKeys)) {
        left = true;
    }
    if (isHeld(state, keySettings.rightKeys)) {
        right = true;
    }
    if (isHeld(state, keySettings.upKeys)) {
        up = true;
    }
    if (isHeld(state, keySettings.downKeys)) {
        down = true;
    }
}

// Change the player's acceleration
void EventHandler::updatePlayer(Player &player) {
    Point newAccel;
    newAccel.x = 0;
    newAccel.y = 0;

    // TODO: It would probably be better to replace left, ect, with newAccel
    if (right) {
        newAccel.x += player.getDAccel().x;
    }
    if (left) {
        newAccel.x -= player.getDAccel().x;
    }

    // TODO: handle these separately, so the player can't fly
    if (up) {
        newAccel.y += player.getDAccel().y;
    }
    if (down) {
        newAccel.y -= player.getDAccel().y;
    }

    // Change the player's acceleration
    player.setAccel(newAccel);

    // Move the player by a pixel
    player.y += move;
    if (move != 0) {
        cout << player.y << "\n";
    }
    move = 0;
}
