#ifndef HOTBAR_HH
#define HOTBAR_HH

#include "Sprite.hh"
#include "UIHelpers.hh"
#include "Action.hh"
#include <vector>
#include <string>

class Inventory;

/* Helper struct. An action pointer and whether it's an item. If it is, then
it should be removed if it can't be found in the inventory. */
struct HotbarItem {
    Action *action;
    bool isItem;
};

/* The hotbar consists of 24 slots that action shortcuts can be stored. These
action shortcuts can be items, skills, or maybe other things I haven't added 
yet. It has two vectors with 12 each. 12 go with the number keys plus - and =,
and 12 go with F1 - F12. They should each be displayed in three groups of four,
like the F keys are on most keyboards. The two groups of 12 should be slightly 
overlapping, with only the front one responding to mouse input, but with both
responding to keyboard input. */
class Hotbar {
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

    // The sprite that holds a bunch of frames rendered onto a texture
    Sprite sprite;
    // Whether anyhting about the hotbar has changed since the sprite was last
    // rendered.
    bool isSpriteUpdated;

    // Which frame is selected
    int selected;

    // What actions are held in the hotbar slots
    std::vector<HotbarItem> actions;

public: // This is public because it gets updated by EventHandler
    // For telling whether any of the buttons were clicked
    std::vector<MouseBox> clickBoxes;

private:
    // Create a texture and render all the textures to it, using the spacing 
    // variables from hotbar. The texture to is expected to have the correct 
    // width and height, and the vector is expected to have length 12.
    SDL_Texture *renderHotbarPart(std::vector<Sprite> textures, 
        SDL_Texture *texture) const;

    // Draw the entire hotbar sprite to a texture. This only needs to be called
    // when the hotbar is first made, or when anything about it changes.
    void updateSprite();

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
    bool update(Action *mouse);

    /* Get rid of items that no longer exist. */
    void update(Inventory &inv, Action *mouse);

    // Return the pointer to the selected action
    Action *getSelected();

    // Draw to the screen
    void render();

    /* Set isSpriteUpdated to false. */
    inline void touch() {
        isSpriteUpdated = false;
    }
};

#endif
