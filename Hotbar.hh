#ifndef HOTBAR_HH
#define HOTBAR_HH

#include "Sprite.hh"

/* The hotbar consists of 24 slots that action shortcuts can be stored. These
action shortcuts can be items, skills, or maybe other things I haven't added 
yet. It has two vectors with 12 each. 12 go with the number keys plus - and =,
and 12 go with F1 - F12. They should each be displayed in three groups of four,
like the F keys are on most keyboards. The two groups of 12 should be slightly 
overlapping, with only the front one responding to mouse input, but with both
responding to keyboard input. */
class Hotbar {

public:
    Sprite frame;
    // How to space the slots
    int smallGap;
    int largeGap;
    // Where to put the inactive slots
    int offsetDown;
    int offsetRight;
    // The sprite that holds a bunch of frames rendered onto a texture
    Sprite frames;

    // Constructor
    Hotbar();
};

#endif
