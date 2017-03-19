#include <iostream>
#include "Hotbar.hh"

using namespace std;

// Constructor, which fills it with default values
Hotbar::Hotbar(void) {
    // If you want to change these default settings, this is the place in the 
    // code to do it.
    smallGap = 4;
    largeGap = 16;
    offsetRight = 8;
    offsetDown = 8;
    // Where to draw the hotbar
    x = 20;
    y = 10;

    frame.name = "frame.png";
    frameSelected.name = "frame_selected.png";
    // How big are the squares in the hotbar
    frame.width = 32;
    frame.height = 32;

    sprite.name = "";
    sprite.width = 12 * frame.width + 12 * smallGap + 2 * largeGap 
        + offsetRight;
    sprite.height = frame.height + offsetDown;

    frame.texture = NULL;
    sprite.texture = NULL;

    isSpriteUpdated = false;
    isSwitched = false;
    selected = 0;
}

// Toggle which row is on top
void Hotbar::toggle() {
    isSwitched = !isSwitched;
    isSpriteUpdated = false;
}

void Hotbar::select(int slot) {
    selected = slot;
    isSpriteUpdated = false;
}
