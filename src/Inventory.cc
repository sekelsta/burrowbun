#include <cassert>
#include <iostream>
#include "Inventory.hh"
#include "filepaths.hh"

using namespace std;

void Inventory::updateSprite(string path) {
    // Tell the renderer to draw to the texture
    sprite.texture -> SetRenderTarget();

    // Now loop through each square twice, once to draw the background and
    // once to draw the frame.
    // Draw the background
    squareSprite.renderGrid(getWidth(), getHeight());

    // Render the items
    // Create a rectangle to draw them to
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = squareSprite.getWidth();
    rectTo.h = squareSprite.getHeight();
    // Rectangle that sits in each square, to refer to when the item isn't
    // the same size as the square
    SDL_Rect refRect = rectTo;
    // Loop through and render each item
    for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
            // Create a spriterect from the item and render it, if the item
            // exists
            Item *item = getItem(row, col);
            if (item != NULL) {
                item -> render(refRect, path);
            }
            else if (isTrash) {
                int w = trashSprite.getWidth();
                int h = trashSprite.getHeight();
                SDL_Rect rectTo = {refRect.x + (refRect.w - w) / 2,
                    refRect.y + (refRect.h - h) / 2, w, h};
                trashSprite.render(rectTo);
            }
            refRect.x += refRect.w;
        }
        refRect.x = 0;
        refRect.y += refRect.h;
    }

    // Now render the frames
    frameSprite.renderGrid(getWidth(), getHeight());

    // And now the sprite is updated
    isSpriteUpdated = true;

    /* Make sure the render target is set to render to the window again. */
    SDL_SetRenderTarget(Renderer::renderer, NULL);
}


void Inventory::useMouse(Item *&mouse, int row, int col) {
    /* Handle left clicks. */
    if (clickBoxes[row][col].event.button == SDL_BUTTON_LEFT) {
        // Switch the items
        Item *temp = mouse;
        if (isTrash && mouse) {
            delete items[row][col];
            mouse = nullptr;
        }
        else {
            mouse = items[row][col];
        }
        items[row][col] = temp;
        /* Add the item being held to the stack, if possible and if 
        there's an item in the slot. If there isn't an item in the
        slot, we can assume the player was trying to pick up an item. */
        if (items[row][col] != NULL) {
            mouse = add(mouse, row, col);
        }
    }
    else if (clickBoxes[row][col].event.button == SDL_BUTTON_RIGHT) {
        /* On right clicks, if holding an item set down one, otherwise pick
        up half the stack. */
        if (mouse) {
            items[row][col] = mouse->merge(items[row][col], -1);
        }
        else if (items[row][col]) {
            int n = items[row][col]->getStack();
            mouse = items[row][col]->merge(mouse, n / 2 - n);
        }
    
    }
}


void Inventory::update_internal(Action *&mouse) {
    for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
            // Ignore buttonup
            if (clickBoxes[row][col].wasClicked && !clickBoxes[row][col].isHeld
                    && clickBoxes[row][col].event.type == SDL_MOUSEBUTTONDOWN) {
                /* If the mouse is holding something but it's not an item,
                ignore it. */
                if (mouse != NULL && !(mouse -> isItem())) {
                    // Abort
                    break;
                }
                /* Now if the mouse is holding anything it's definitely an 
                item. */
                touch();
                Item **mouseitem = (Item **)&mouse;
                useMouse(*mouseitem, row, col);
            }
        }
    }
}

// Constructors
Inventory::Inventory(int cols, int rows, string path) : 
    Inventory(cols, rows, path, false) {}

Inventory::Inventory(int cols, int rows, string path, bool trash) {
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

    /* Sprite starts off not updated. */
    isSpriteUpdated = false;

    /* Pick a background color. */
    Light squareColor;
    squareColor.r = 128;
    squareColor.g = 128;
    squareColor.b = 255;

    // TODO: fix having hard-coded sprite info. 
    /* x, y, w, h, name */
    squareSprite = Sprite(0, 0, ACTION_SPRITE_SIZE, ACTION_SPRITE_SIZE, 
        "inventory.png");
    frameSprite = Sprite(ACTION_SPRITE_SIZE, 0, ACTION_SPRITE_SIZE, 
        ACTION_SPRITE_SIZE, "inventory.png");
    trashSprite = Sprite(0, 0, 16, 16, "trash.png");

    // And actually load it
    squareSprite.loadTexture(path + UI_SPRITE_PATH);
    frameSprite.loadTexture(path + UI_SPRITE_PATH);
    trashSprite.loadTexture(path + UI_SPRITE_PATH);

    /* Assign the squareSprite a different color. */
    squareSprite.setColorMod(squareColor);

    /* Actually set the clickboxes to what they should be. */
    updateClickBoxes();    

    /* Make a texture of the right size. */
    Uint32 pixelFormat = squareSprite.texture -> getFormat();
    int width = squareSprite.getWidth() * getWidth();
    int height = squareSprite.getHeight() * getHeight();
    sprite.texture.reset(new Texture(pixelFormat,
            SDL_TEXTUREACCESS_TARGET, width, height));
    sprite.rect.w = width;
    sprite.rect.h = height;
    sprite.rect.x = 0;
    sprite.rect.y = 0;

    isTrash = trash;
}

/* Copy constructor. Don't use, it just asserts false. If I ever think of a 
good reason anyone might want to use it, I'll write one that does that. */
Inventory::Inventory(const Inventory &toCopy) {
    *this = toCopy;
}

/* operator=, also don't use, also just asserts false. */
Inventory &Inventory::operator=(const Inventory &toCopy) {
    /* If this were actually going to do anything I would remember to check
    for self-assignment here. */
    assert(false);
    return *this;
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

    /* Otherwise, try to add it to whatever's in the slot. */
    return items[row][col] -> merge(item, 0);   
}

// Take an item and put it in the first empty slot of the inventory. Return 
// the item if it doesn't fit or NULL if it does.
Item *Inventory::pickup(Item *item) {
    /* Loop through the inventory looking for a stack to merge. */
    item = stack(item);

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


Item *Inventory::stack(Item *item) {
    /* Loop through the inventory looking for a stack to merge. */
    for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
            // Return NULL if we can successfully add it to this spot
            if (items[row][col]) {
                item = items[row][col] -> merge(item, 0);
            }
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
            clickBoxes[row][col].w = Inventory::squareSprite.getWidth();
            clickBoxes[row][col].h = Inventory::squareSprite.getHeight();
            clickBoxes[row][col].x = newX;
            clickBoxes[row][col].y = newY;
            // Also initialize the click info
            clickBoxes[row][col].wasClicked = false;
            clickBoxes[row][col].containsMouse = false;
 
            newX += Inventory::squareSprite.getWidth();
        }
        newX = x;
        newY += Inventory::squareSprite.getHeight();
    }
}

void Inventory::resetClicks() {
    for (unsigned int i = 0; i < clickBoxes.size(); i++) {
        for (unsigned int j = 0; j < clickBoxes[i].size(); j++) {
            clickBoxes[i][j].wasClicked = false;
        }
    }
}

// Use mouse input
void Inventory::update(Action *&mouse) {
    update_internal(mouse);
    resetClicks();
}

void Inventory::update() {
    for (unsigned int i = 0; i < items.size(); i++) {
        for (unsigned int j = 0; j < items[i].size(); j++) {
            if (items[i][j] != nullptr && items[i][j] -> getStack() <= 0) {
                delete items[i][j];
                items[i][j] = nullptr;
                touch();
            }
        }
    }
}

void Inventory::render(string path) {
    if (!isSpriteUpdated) {
        updateSprite(path);
    }

    /* Set the rect to draw to. */
    SDL_Rect rectTo;
    rectTo.w = sprite.getWidth();
    assert(rectTo.w == squareSprite.getWidth() * getWidth());
    rectTo.h = sprite.getHeight();
    assert(rectTo.h == squareSprite.getHeight() * getHeight());
    rectTo.x = x;
    rectTo.y = y;

    // And actually render
    sprite.render(rectTo);

}

bool Inventory::contains(Item *item) {
    for (unsigned int i = 0; i < items.size(); i++) {
        for (unsigned int j = 0; j < items[i].size(); j++) {
            if (items[i][j] == item) {
                return true;
            }
        }
    }
    return false;
}
