#ifndef INVENTORY_HH
#define INVENTORY_HH

#include "Sprite.hh"
#include "UIHelpers.hh"
#include "Light.hh"
#include "Item.hh"
#include <vector>
#include <string>

#define INVENTORY_COLOR {128, 128, 255, 255}

/* An inventory is basically just a thing that holds items. */
class Inventory {
protected:
    /* Whether to destroy items or switch them. */
    bool isTrash;

    // For storing the list of items
    std::vector<std::vector<Item*>> items;

    /* Have an up-to-date sprite. */
    void updateSprite(std::string path);

public:
    // Where the top left corner of the inventory is
    int x;
    int y;

    // For storing information about where has been clicked
    std::vector<std::vector<MouseBox>> clickBoxes;

protected:
    /* The sprite of the square to use for the background, which other than
    the color is the same for all inventories. */
    Sprite squareSprite;
    Sprite frameSprite;
    Sprite trashSprite;

    // The sprite of all the things, put together
    Sprite sprite;
    // Whether the sprite is updated
    bool isSpriteUpdated;

    /* Use mouse input on a given square. */
    void useMouse(Item *&mouse, int row, int col);

    /* Implementation of update, for use by child classes that don't want to
    reset the clickboxes afterwards. */
    void update_internal(Action *&mouse);

public:
    // Constructors
    Inventory(int cols, int rows, std::string path);
    Inventory(int cols, int rows, std::string path, bool trash);

    /* Copy constructor. Don't use; all it does it assert false. If I ever
    think of any good reason why anyone would use a copy constructor of an
    inventory, and a way it could be implemented that won't make everything go 
    horribly wrong, I'll write a proper one. */
    Inventory(const Inventory &toCopy);

    /* operator=. Also don't use, also just asserts false. */
    Inventory &operator=(const Inventory &toCopy);

    // Destructor
    virtual ~Inventory();

    // Access functions
    int getWidth() const;
    int getHeight() const;
    Item *getItem(int row, int col) const;
    void setItem(Item *item, int row, int col);

    // Put the item in the slot, if possible. If not possible, return false.
    Item *add(Item *item, int row, int col);

    // Take an item and put it in the first empty slot of the inventory. Return 
    // the item if it doesn't fit or NULL if it does.
    Item *pickup(Item *item);

    /* Take an item and add it to an existing stack, without putting it in an 
    empty space. Return whatever we can't add. */
    Item *stack(Item *item);

    // Call this after changing x or y, puts clickboxes in the right place
    void updateClickBoxes();

    /* Set wasClicked to false for all clickboxes. */
    void resetClicks();

    // Use mouse input
    virtual void update(Action *&mouse);

    /* Delete any items with a stack of 0. */
    void update();

    /* Render itself. */
    virtual void render(std::string path);

    /* Set isSpriteUpdated to false. */
    inline void touch() {
        isSpriteUpdated = false;
    }

    /* Return true if the item is in the inventory. */
    bool contains(Item *item);
};

#endif
