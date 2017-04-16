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

// Change the bool values of a MouseBox vector so they know whether they were
// clicked
void EventHandler::updateMouseBoxes(vector<MouseBox> &mouseBoxes,
        const SDL_Event &event) {
    assert(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONUP
        || event.type == SDL_MOUSEBUTTONDOWN);
    // Mouse coordinates, relative to the window
    int x;
    int y;
    if (event.type == SDL_MOUSEMOTION) {
        x = event.motion.x;
        y = event.motion.y;
    }
    // Else it's a mouse button down or up event
    else {
        x = event.button.x;
        y = event.button.y;
    }

    for (unsigned int i = 0; i < mouseBoxes.size(); i++) {
        // Note that MouseBox.contains(x, y) also sets mouseBox.containsMouse
        // to the appropriate value
        // This if statement sets containsMouse and checks whether it was a
        // button press or just the mouse moving
        if (mouseBoxes[i].contains(x, y) && event.type != SDL_MOUSEMOTION) {
            // If it was a button press in the box, fill in the
            // appropriate fields
            mouseBoxes[i].wasClicked = true;
            mouseBoxes[i].event = event.button;
        }
        // And the mouseBox is responsible for making wasClicked false again,
        // so we don't want to do that here
    }

}

// Update the mouseboxes in an inventory
void EventHandler::updateInventoryClickBoxes(Inventory &inventory, 
        const SDL_Event &event) {
    for (int i = 0; i < inventory.getHeight(); i++) {
        updateMouseBoxes(inventory.clickBoxes[i], event);
    }
}


// Public methods

// Constructor
EventHandler::EventHandler() {
    // Assume the player starts off not moving
    left = false;
    right = false;
    up = false;
    down = false;
    jump = false;

    isJumping = false;
    hasJumped = false;

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
    keySettings.jumpKeys.push_back(SDL_SCANCODE_SPACE);
    keySettings.jumpKeys.push_back(SDL_SCANCODE_KP_SPACE);
    // Keys to open the inventory and whatever else opens along with it
    keySettings.inventoryKeys.push_back(SDL_SCANCODE_I);
    keySettings.inventoryKeys.push_back(SDL_SCANCODE_C);
    // For the hotbar
    keySettings.toggleHotbarKeys.push_back(SDL_SCANCODE_X);
    // And each of 24 keys to select a hotbar slot
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_1);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_2);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_3);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_4);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_5);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_6);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_7);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_8);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_9);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_0);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_MINUS);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_EQUALS);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F1);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F2);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F3);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F4);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F5);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F6);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F7);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F8);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F9);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F10);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F11);
    keySettings.hotbarKeys.push_back(SDL_SCANCODE_F12);
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
void EventHandler::mouseEvent(const SDL_Event &event, Player &player) {
    // Tell the hotbar and inventories whether they were clicked
    if (event.type != SDL_MOUSEWHEEL) {
        updateMouseBoxes(player.hotbar.clickBoxes, event);
        updateInventoryClickBoxes(player.inventory, event);
        updateInventoryClickBoxes(player.trash, event);
    }
}

// Do whatever should be done when key presses or releases happen
void EventHandler::keyEvent(const SDL_Event &event, Player &player) {
    SDL_Scancode key = event.key.keysym.scancode;

    // Here we should handle keys which don't need to be held down to work.
    if (event.type == SDL_KEYUP) {
        // Pass
    }
    else if (isIn(key, keySettings.inventoryKeys)) {
        player.toggleInventory();
    }
    else if (isIn(key, keySettings.toggleHotbarKeys)) {
        player.hotbar.toggle();
    }
    else if (isIn(key, keySettings.hotbarKeys)) {
        // Select the appropriate slot in the hotbar
        // This vector actually has the order matter, so you can't map more
        // than one key to each hotbar slot
        for (unsigned int i = 0; i < keySettings.hotbarKeys.size(); i++) {
            if (key == keySettings.hotbarKeys[i]) {
                player.hotbar.select(i);
            }
        }
    }
    // For testing only, TODO: remove
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
    jump = false;

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
    if (isHeld(state, keySettings.jumpKeys)) {
        jump = true;
    }
    else {
        isJumping = false;
        hasJumped = false;
    }
}

// Change the player's acceleration
void EventHandler::updatePlayer(Player &player) {
    // Update the player's hotbar
    player.hotbar.update(player.mouseSlot);
    player.inventory.update();
    player.trash.update();

    // and update the player's accelleration
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
    if (jump && (player.timeOffGround <= player.maxJumpTime
            || player.maxJumpTime == -1)
            && (isJumping == hasJumped)) {
        newAccel.y += player.getDAccel().y;
        isJumping = true;
        hasJumped = true;
    }
    else {
        isJumping = false;
    }

    // TODO: handle these separately, so the player can't fly
    if (up) {
        newAccel.y += player.getDAccel().y;
    }
    if (down) {
        player.collidePlatforms = false;
    }
    else {
        player.collidePlatforms = true;
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
