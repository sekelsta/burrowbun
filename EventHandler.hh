#ifndef EVENTHANDLER_HH
#define EVENTHANDLER_HH

#include <SDL2/SDL.h>
#include "WindowHandler.hh"
#include "Player.hh"

using namespace std;

/* A struct to hold information about which keys do what. This is so that
   later the player can change these settings. */
struct KeySettings {
    // Which keys are for movement
    vector<SDL_Scancode> leftKeys, rightKeys, upKeys, downKeys;
};

/* A class to handle events such as keyboard input or mouse movement. */
class EventHandler {
    // Which keys do what
    KeySettings keySettings;

    // Whether the player is trying to move in some direction
    bool left, right, up, down;

    // To move by one pixel at a time, in the vertical direction 
    // This currently only exists for debugging
    int move;

    // Whether certain windows have been opened
    bool inventory, quests;

    // Helper functions

    // Tell whether a scancode is in a vector
    bool isIn(SDL_Scancode key, vector<SDL_Scancode> keys);

    // Tell whether a vector has a key that's being held down
    bool isHeld(const Uint8 *state, vector<SDL_Scancode> keys);

public:
    // Constructor
    EventHandler();

    // Access methods
    KeySettings getKeySettings();
    void setKeySettings(KeySettings &newSettings); // non-ideal name

    // Handle events
    void windowEvent(const SDL_Event &event, bool &isFocused,
                            WindowHandler &window);

    // Do whatever should be done when a mouse event happens
    void mouseEvent(const SDL_Event &event);

    // Do whatever should be done when a key is pressed or released
    void keyEvent(const SDL_Event &event);

    // Do stuff for keys being held down
    void updateKeys(const Uint8 *state);

    // Tell the Player what its trying to do
    void updatePlayer(Player &player);
};

#endif
