#ifndef DROPPED_ITEM_HH
#define DROPPED_ITEM_HH

#include "Movable.hh"
#include "Item.hh"

class DroppedItem: public movable::Movable {
public:
    Item *item;

    DroppedItem(Item *item, int x, int y, int worldWidth);
    ~DroppedItem();

    /* Render itself. */
    virtual void render(const Rect &camera);

    /* Merge with another stack. */
    virtual void merge(DroppedItem *item);
};

#endif
