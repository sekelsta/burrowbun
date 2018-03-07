#ifndef DROPPED_ITEM_HH
#define DROPPED_ITEM_HH

#include "Movable.hh"
#include "Item.hh"

#define ITEM_MERGE_DISTANCE 64
#define ITEM_ATTRACT_SPEED 8.0

class DroppedItem: public movable::Movable {
    bool attracting;
public:
    Item *item;

private:
    inline void updateItem() {
        /* If we don't have an item, delete it. */
        if (item && item->getStack() == 0) {
            delete item;
            item = nullptr;
        }
    }

public:
    DroppedItem(Item *item, int x, int y, int worldWidth);
    ~DroppedItem();

    /* Render itself. */
    virtual void render(const Rect &camera);

    /* Merge with another stack. */
    void merge(DroppedItem *item);

    /* Move in a direction. */
    virtual void attract(int x, int y, double xspeed, double yspeed);

    /* Self-explanatory. */
    virtual void update();
};

#endif
