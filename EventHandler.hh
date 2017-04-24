#ifndef EVENTHANDLER_HH
#define EVENTHANDLER_HH

#include <vector>
#include <SDL2/SDL.h>

// forward declare
class WindowHandler;
class Player;
class Inventory;
class Hotbar;
class Map;
struct MouseBox;
struct StatBar;

using namespace std;

/* A struct to hold information about which keys do what. This is so that
   later the player can change these settings. */
struct KeySettings {
    // Which keys are for movement
    vector<SDL_Scancode> leftKeys, rightKeys, upKeys, downKeys, jumpKeys;

    // Key to open the inventory and whatever opens along with it
    vector<SDL_Scancode> inventoryKeys;

    // Key to toggle the hotbar
    vector<SDL_Scancode> toggleHotbarKeys;

    // 24 keys to select a hotbar slot
    vector<SDL_Scancode> hotbarKeys;
};

/* A class to handle events such as keyboard input or mouse movement. */
class EventHandler {
    // Which keys do what
    KeySettings keySettings;

    // Whether the player is trying to move in some direction
    bool left, right, up, down, jump;
    // Veriables to keep track of the one jump per key press rule
    bool isJumping;
    bool hasJumped; // as in, since the last time the key was pressed

    // The state of the mouse
    bool isLeftButtonDown;
    bool isRightButtonDown;
    bool wasLeftButtonDown;
    bool wasRightButtonDown;

    // To move by one pixel at a time, in the vertical direction 
    // This currently only exists for debugging
    int move; // TODO: remove

    // Helper functions

    // Tell whether a scancode is in a vector
    bool isIn(SDL_Scancode key, vector<SDL_Scancode> keys);

    // Tell whether a vector has a key that's being held down
    bool isHeld(const Uint8 *state, vector<SDL_Scancode> keys);

    // Change the bool values of a MouseBox vector so they know whether they 
    // were clicked
    // Return true if the mouse clicked any of the boxes
    bool updateMouseBoxes(vector<MouseBox> &mouseBoxes);

    // Update the mouseboxes of an inventory
    // Return true if the mouse clicked any of the boxes
    bool updateInventoryClickBoxes(Inventory &inventory);

public:
    // Constructor
    EventHandler();

    // Access methods
    KeySettings getKeySettings();
    void setKeySettings(KeySettings &newSettings); // non-ideal name

    // Handle events
    void windowEvent(const SDL_Event &event, bool &isFocused,
                            WindowHandler &window);

    // Update the state of the mouse
    void mouseEvent(const SDL_Event &event);

    // Do whatever should be done when a mouse event happens
    void useMouse(Player &player, Map &map);

    // Do whatever should be done when a key is pressed or released
    void keyEvent(const SDL_Event &event, Player &player);

    // Do stuff for keys being held down
    void updateKeys(const Uint8 *state);

    // Tell the Player what its trying to do
    void updatePlayer(Player &player);

    // Do all the stuff that needs to be done every frame
    void update(Player &player, Map &map);
};

#endif
