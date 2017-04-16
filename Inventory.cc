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

// Add the item to the slot, if possible. If not possible, return false.
bool Inventory::add(Item *item, int row, int col) {
    // If there's nothing in the slot, we can definately add it.
    if (items[row][col] == NULL) {
        setItem(item, row, col);
        return true;
    }

    // If they're different types of items, then they definately don't stack
    if (items[row][col] -> getType() != item -> getType()) {
        return false;
    }

    // TODO: stack items if possible
    return false;
    
}

// Take an item and put it in the first empty slot of the inventory. Return 
// false if it doesn't fit.
bool Inventory::pickup(Item *item) {
    // TODO: attempt to stack items whenever possible
    // Loop through the inventory looking for a space
    for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
            // Return true if we can successfully add it to this spot
            if (add(item, row, col)) {
                return true;
            }
        }
    }

    // If we've looped through the whole inventory and not sucessfully added 
    // it, then we can't.
    return false;
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
void Inventory::update() {
    for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
            if (clickBoxes[row][col].wasClicked) {
                // TODO: put item
                clickBoxes[row][col].wasClicked = false;
            }
        }
    }
}
