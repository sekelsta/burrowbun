#ifndef HOTBAR_HH
#define HOTBAR_HH

#include "../render/Sprite.hh"
#include "Button.hh"
#include "../Action.hh"
#include "Inventory.hh"
#include <vector>
#include <string>

/* The hotbar consists of 24 slots that action shortcuts can be stored. These
action shortcuts can be items, skills, or maybe other things I haven't added 
yet. It has two vectors with 12 each. 12 go with the number keys plus - and =,
and 12 go with F1 - F12. They should each be displayed in three groups of four,
like the F keys are on most keyboards. The two groups of 12 should be slightly 
overlapping, with only the front one responding to mouse input, but with both
responding to keyboard input. */
class Hotbar : public Inventory {
private:
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

    // Which frame is selected
    int selected;

    /* What color for selected and unselected squares. */
    Light selectColor;
    Light unselectColor;

    // Create a texture and render all the textures to it, using the spacing 
    // variables from hotbar. row is expected to be 0 or 1.
    Texture *renderHotbarPart(int row, std::string path, 
        Texture *texture, int left, int up);

    // Draw the entire hotbar sprite to a texture. This only needs to be called
    // when the hotbar is first made, or when anything about it changes.
    void updateSprite(std::string path);

public:
    // Constructor
    Hotbar(std::string path);

    // Access functions
    inline int getX() {
        return xStart;
    }

    inline int getY() {
        return yStart;
    }

    inline int getWidth() {
        return sprite.getWidth();
    }

    inline int getHeight() {
        return sprite.getHeight();
    }

    // Select a slot
    void select(int slot);

    // Use mouse input, return true if the item should be put in the inventory
    void update(Action *&mouse, bool isInvOpen);

    // Return the pointer to the selected action
    Action *getSelected();

    // Draw to the screen
    void render(std::string path);
};

#endif
