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


    frame.name = "frame.png";
    // How big are the squares in the hotbar
    frame.width = 32;
    frame.height = 32;

    frames.name = "";
    frames.width = 12 * frame.width + 9 * smallGap + 2 * largeGap + offsetRight;
    frames.height = frame.height + offsetDown;

    frame.texture = NULL;
    frames.texture = NULL;
}
