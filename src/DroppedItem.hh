#ifndef DROPPED_ITEM_HH
#define DROPPED_ITEM_HH

#include "Movable.hh"
#include "Item.hh"

class DroppedItem: public movable::Movable {
    Item *item;

public:
    DroppedItem(Item *item, int x, int y);
};

#endif
