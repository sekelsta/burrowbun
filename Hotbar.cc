#include <iostream>
#include "Hotbar.hh"

// The number of slots in the hotbar

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
    xStart = 20;
    yStart = 10;

    frame.name = "frame.png";
    // How big are the squares in the hotbar
    frame.width = 32;
    frame.height = 32;
    frame.rows = 1;
    frame.cols = 2;
    frame.row = 0;
    frame.col = 0;

    sprite.name = "";
    sprite.width = 12 * frame.width + 12 * smallGap + 2 * largeGap 
        + offsetRight;
    sprite.height = frame.height + offsetDown;

    frame.texture = NULL;
    sprite.texture = NULL;

    isSpriteUpdated = false;
    isSwitched = false;
    selected = 0;

    clickBoxes.resize(12);

    int x = xStart + offsetRight;
    // For each section of four
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            int index = 4 * i + j;
            clickBoxes[index].x = x;
            clickBoxes[index].y = yStart + offsetDown;
            clickBoxes[index].w = frame.width;
            clickBoxes[index].h = frame.height;
            clickBoxes[index].wasClicked = false;
            clickBoxes[index].containsMouse = false;
            x += frame.width + smallGap;
        }
        x += largeGap;
    }


}

// Toggle which row is on top
void Hotbar::toggle() {
    isSwitched = !isSwitched;
    isSpriteUpdated = false;
}

// Select a slot
void Hotbar::select(int slot) {
    selected = slot;
    isSpriteUpdated = false;
}

// Use mouse input
void Hotbar::update() {
    for (unsigned int i = 0; i < clickBoxes.size(); i++) {
        if (clickBoxes[i].wasClicked) {
            // Ignore the difference between buttondown and buttonup for now
            // Add 12 to i if isSwitched
            select(i + 12 * (int)isSwitched);
            // Now we've use the click for this update
            clickBoxes[i].wasClicked = false;
        }
    }
}

