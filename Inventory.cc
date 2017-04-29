#include <cassert>
#include <iostream>
#include "Inventory.hh"

// Constructor
Inventory::Inventory(int cols, int rows) {
    // Initialize the location
    x = 0;
    y = 0;

    items.resize(rows);
    clickBoxes.resize(rows);
    for (int i = 0; i < rows; i++) {
        clickBoxes[i].resize(cols);
        items[i].resize(cols);
        // Set every item* to NULL
        for (int j = 0; j < cols; j++) {
            items[i][j] = NULL;
        }
    }

    // Set all texture pointers to null
    sprite.texture = NULL;
    isSpriteUpdated = false;

    // Pick a background color
    squareColor.r = 128;
    squareColor.g = 128;
    squareColor.b = 255;
}

/* Copy constructor. Don't use, it just asserts false. If I ever think of a 
good reason anyone might want to use it, I'll write one that does that. */
Inventory::Inventory(const Inventory &toCopy) {
    *this = toCopy;
}

/* operator=, also don't use, also just asserts false. */
Inventory Inventory::operator=(const Inventory &toCopy) {
    assert(false);
}

/* Destructor. Assumes this inventory is the only object with pointers to 
the items it contains. */
Inventory::~Inventory() {
    for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
            // Destroy each item, if it exists
            if (items[row][col] != NULL) {
                delete items[row][col];
            }
        }
    }
}

// Access functions

// Returns the number of columns in the inventory
int Inventory::getWidth() const {
    assert(items.size() > 0);
    return items[0].size();
}

// Return the number of rows
int Inventory::getHeight() const {
    return items.size();
}

// Get the item held at a certain location
Item *Inventory::getItem(int row, int col) const {
    assert(row < getHeight());
    assert(col < getWidth());
    return items[row][col];
}

// Tell a certain location to contain some item
void Inventory::setItem(Item *item, int row, int col) {
    assert(row < getHeight());
    assert(col < getWidth());
    items[row][col] = item;
    isSpriteUpdated = false;
}

// Add the item to the slot, if possible. Return whatever was not able to be
// added (NULL if that slot could hold everything being added).
Item *Inventory::add(Item *item, int row, int col) {
    // If there's nothing in the slot, we can definately add it.
    if (items[row][col] == NULL) {
        setItem(item, row, col);
        isSpriteUpdated = false;
        return NULL;
    }

    // If we're not adding an item, it's successful.
    if (item == NULL) {
        return NULL;
    }

    // If they're different types of items, then they definately don't stack
    if (items[row][col] -> getType() != item -> getType()) {
        return item;
    }

    // TODO: stack items if possible
    return item;
    
}

// Take an item and put it in the first empty slot of the inventory. Return 
// the item if it doesn't fit or NULL if it does.
Item *Inventory::pickup(Item *item) {
    // TODO: attempt to stack items whenever possible
    // Loop through the inventory looking for a space
    for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
            // Return NULL if we can successfully add it to this spot
            item = add(item, row, col);
            // If item is NULL now, then we're done.
            if (item == NULL) {
                return NULL;
            }
        }
    }

    // If we've looped through the whole inventory and not sucessfully added 
    // it, then we can't.
    return item;
}

// Call this after changing x, y, width, or height
// Puts the clickboxes rects in the right place
void Inventory::updateClickBoxes() {
    int newX = x;
    int newY = y;
    for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
            clickBoxes[row][col].w = Inventory::squareSprite.width;
            clickBoxes[row][col].h = Inventory::squareSprite.height;
            clickBoxes[row][col].x = newX;
            clickBoxes[row][col].y = newY;
            // Also initialize the click info
            clickBoxes[row][col].wasClicked = false;
            clickBoxes[row][col].containsMouse = false;
 
            newX += Inventory::squareSprite.width;
        }
        newX = x;
        newY += Inventory::squareSprite.height;
    }
}

// Use mouse input
void Inventory::update(Action *&mouse) {
    for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
            // Ignore buttonup
            if (clickBoxes[row][col].wasClicked && !clickBoxes[row][col].isHeld
                    && clickBoxes[row][col].event.type == SDL_MOUSEBUTTONDOWN) {
                // Now we've used the input for this clickBox
                clickBoxes[row][col].wasClicked = false;
                // If the mouse is holding something but it's not an item,
                // ignore it
                if (mouse != NULL && !(mouse -> isItem)) {
                    // Abort
                    break;
                }
                // Now we don't have to worry that the mouse might be holding
                // something that isn't an item
                // TODO: quit assuming it was a left click
                // Switch the items
                isSpriteUpdated = false;
                Item *item = (Item *)mouse;
                mouse = items[row][col];
                items[row][col] = item;
                // Add the item being held to the stack, if possible and if 
                // there's an item in the slot. If there isn't an item in the
                // slot, we can assume the player was trying to pick up an item.
                if (items[row][col] != NULL) {
                    mouse = add((Item *)mouse, row, col);
                }
            }
        }
    }
}
