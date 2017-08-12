#ifndef HOTBAR_HH
#define HOTBAR_HH

#include <vector>
#include "Sprite.hh"
#include "UIHelpers.hh"
#include "Action.hh"

/* The hotbar consists of 24 slots that action shortcuts can be stored. These
action shortcuts can be items, skills, or maybe other things I haven't added 
yet. It has two vectors with 12 each. 12 go with the number keys plus - and =,
and 12 go with F1 - F12. They should each be displayed in three groups of four,
like the F keys are on most keyboards. The two groups of 12 should be slightly 
overlapping, with only the front one responding to mouse input, but with both
responding to keyboard input. */
class Hotbar {

public:
    // For drawing a frame around every hotbar item
    Sprite frame;
    // How to space the slots
    int smallGap;
    int largeGap;
    // Where to put the inactive slots
    int offsetDown;
    int offsetRight;
    // Where to draw the hotbar
    int xStart;
    int yStart;

    // The sprite that holds a bunch of frames rendered onto a texture
    Sprite sprite;
    // Whether anyhting about the hotbar has changed since the sprite was last
    // rendered.
    bool isSpriteUpdated;

    // Which frame is selected
    int selected;

    // Whether the back hotbar was brought to the front
    bool isSwitched;

    // What actions are held in the hotbar slots
    std::vector<Action *> actions;

    // For telling whether any of the buttons were clicked
    std::vector<MouseBox> clickBoxes;

    // Constructor
    Hotbar();

    // Switch the two rows
    void toggle();

    // Select a slot
    void select(int slot);

    // Use mouse input, return true if the item should be put in the inventory
    bool update(Action *mouse);

    // Return the pointer to the selected action
    Action *getSelected();
};

#endif
