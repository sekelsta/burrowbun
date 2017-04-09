#ifndef INVENTORY_HH
#define INVENTORY_HH

#include <vector>
#include "Sprite.hh"
#include "UIHelpers.hh"
#include "Light.hh"

using namespace std;

class Item;

/* An inventory is basically just a thing that holds items. */
class Inventory {
    // For storing the list of items
    vector<vector<Item*>> items;

public:
    // Where the top left corner of the inventory is
    int x;
    int y;

    // For storing information about where has been clicked
    vector<vector<MouseBox>> clickBoxes;

    /* The sprite of the square to use for the background, which other than
    the color is the same for all inventories. */
    static Sprite squareSprite;

    /* Color with which to modulate the background squares. */
    Light squareColor;

    // The sprite of all the things, put together
    Sprite sprite;
    // Whether the sprite is updated
    bool isSpriteUpdated;

    // Constructor, given the size
    Inventory(int cols, int rows);

    // Access functions
    int getWidth() const;
    int getHeight() const;
    Item *getItem(int, int) const;
    void setItem(Item *, int, int);

    // Call this after changing x or y, puts clickboxes in the right place
    void updateClickBoxes();
};

#endif
