#ifndef EVENTHANDLER_HH
#define EVENTHANDLER_HH

#include <SDL2/SDL.h>
#include "WindowHandler.hh"
#include "Player.hh"

using namespace std;

/* A class to handle events such as keyboard input or mouse movement. */
class EventHandler {
    // Whether the player is trying to move in some direction
    bool left, right, up, down;

    // Whether certain windows have been opened
    bool inventory, quests;

    // Helper functions

    // Tell whether a keycode is in a vector
    bool isIn(SDL_Keycode key, vector<SDL_Keycode> keys);

public:
    // Handle events
    void windowEvent(const SDL_Event &event, bool &isFocused,
                            WindowHandler &window);

    // Do whatever should be done when a mouse event happens
    void mouseEvent(const SDL_Event &event);

    // Do whatever should be done when a key is pressed or released
    void keyEvent(const SDL_Event &event, Player &player);
};

#endif
