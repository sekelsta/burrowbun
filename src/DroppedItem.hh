#ifndef DROPPED_ITEM_HH
#define DROPPED_ITEM_HH

#include "Movable.hh"
#include "Item.hh"

#define ITEM_MERGE_DISTANCE 64
#define ITEM_ATTRACT_SPEED 8.0
#define ITEM_THROW_SPEED 8.0

class DroppedItem: public movable::Movable {
    bool attracting;
    int throwticks;
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

    /* Be thrown. dir = 0 for left or 1 for right. */
    inline void toss(bool dir, int dist) {
        int d = 2 * dir - 1;
        // TODO: if I want this to throw at just the right speed, I'll need
        // to do a calculation involving drag.
        throwticks = 2 * dist / ITEM_THROW_SPEED;
        velocity.x = d * ITEM_THROW_SPEED 
            + (rand() % (int)(ITEM_THROW_SPEED * 10))/ 20.0;
        velocity.y = ITEM_THROW_SPEED 
            + (rand() % (int)(ITEM_THROW_SPEED * 10)) / 20.0;
    }

    /* Move in a direction. */
    virtual void attract(int x, int y, double xspeed, double yspeed);

    /* Self-explanatory. */
    virtual void update();

    /* Access function */
    bool canPickup() {
        return !throwticks;
    }
};

#endif
