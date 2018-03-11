#ifndef EVENTHANDLER_HH
#define EVENTHANDLER_HH

#include <vector>
#include <SDL2/SDL.h>

// forward declare
class WindowHandler;
class Player;
class Inventory;
class Hotbar;
class World;
struct MouseBox;
struct StatBar;
class Menu;
class DroppedItem;

/* A struct to hold information about which keys do what. This is so that
   later the player can change these settings. */
struct KeySettings {
    // Which keys are for movement
    std::vector<SDL_Scancode> leftKeys, rightKeys, upKeys, downKeys, jumpKeys;

    // Keys to open the inventory and whatever opens along with it
    std::vector<SDL_Scancode> inventoryKeys;

    // 24 keys to select a hotbar slot
    std::vector<SDL_Scancode> hotbarKeys;

    /* Keys to toss items onto the ground. */
    std::vector<SDL_Scancode> tossKeys;
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
    // For when the button is pressed and released multiple times in a frame
    int leftClicks;
    int rightClicks;


    // To move by one pixel at a time, in the vertical direction 
    // This currently only exists for debugging
    int move; // TODO: remove

    // Helper functions

    // Tell whether a scancode is in a vector
    bool isIn(SDL_Scancode key, std::vector<SDL_Scancode> keys);

    // Tell whether a vector has a key that's being held down
    bool isHeld(const Uint8 *state, std::vector<SDL_Scancode> keys);

public:
    // Update a single mouse box
    bool updateBox(MouseBox &box);

    // Change the bool values of a MouseBox vector so they know whether they 
    // were clicked
    // Return true if the mouse clicked any of the boxes
    bool updateMouseBoxes(std::vector<MouseBox> &mouseBoxes);

    // Update the mouseboxes of an inventory
    // Return true if the mouse clicked any of the boxes
    bool updateInventoryClickBoxes(Inventory &inventory);

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
    void useMouse(Player &player, World &world);

    // Do whatever should be done when a key is pressed or released
    void keyEvent(const SDL_Event &event, Player &player, 
        std::vector<DroppedItem *> &drops);

    // Do stuff for keys being held down
    void updateKeys(const Uint8 *state);

    // Tell the Player what its trying to do
    void updatePlayer(Player &player);

    // Update the menu's mouseboxes
    void updateMenu(Menu &menu);

    // Do all the stuff that needs to be done every frame
    void update(World &world);
};

#endif
