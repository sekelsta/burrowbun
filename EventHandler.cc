#include <cassert>
#include <iostream>
#include "EventHandler.hh"

using namespace std;

bool EventHandler::isIn(SDL_Keycode key, vector<SDL_Keycode> keys) {
    for (unsigned i = 0; i < keys.size(); i++) {
        if (keys[i] == key) {
            return true;
        }
    }

    // None of the keys in the vector match the key
    return false;
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
void EventHandler::keyEvent(const SDL_Event &event, Player &player) {
    SDL_Keycode key = event.key.keysym.sym;
    // Maybe keySettings should belong to this class
    KeySettings keySettings = player.getKeySettings();
    if (isIn(key, keySettings.leftKeys)) {
        player.xAccel -= 1;
    }
    else if (isIn(key, keySettings.rightKeys)) {
        player.xAccel += 1;
    }
    if (isIn(key, keySettings.upKeys)) {
        player.yAccel += 1;
    }
    else if (isIn(key, keySettings.downKeys)) {
        player.yAccel -= 1;
    }
}
