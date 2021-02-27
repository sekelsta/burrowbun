#include "DroppedItem.hh"

using namespace std;

DroppedItem::DroppedItem(Item *i, int x, int y, int worldWidth) {
    item = i;
    setX(x);
    setY(y);
    rect.w = i->sprite.getWidth();
    rect.h = i->sprite.getHeight();
    rect.worldWidth = worldWidth;
    nextRect = rect;
    nextRect.x = 0;
    nextRect.y = 0;

    // TODO: remove magic numbers
    drag = {0.95, 0.9166};
    attracting = false;
    throwticks = 0;
}

DroppedItem::~DroppedItem() {
    delete item;
}

void DroppedItem::render(const Rect &camera) {
    if (!item) {
        return;
    }
    // Make sure the renderer draw color is set to white
    Renderer::setColorWhite();

    SDL_Rect to = {rect.x, rect.y, rect.w, rect.h};
    convertRect(to, camera);
    item -> sprite.render(to);
}

void DroppedItem::merge(DroppedItem *dropped) {
    if (!item || !dropped->item) {
        return;
    }
    if (rect.intersects(dropped->rect)) {
        dropped->item = item->merge(dropped->item, 0);
        updateItem();
        dropped -> updateItem();
    }
    else if (item -> getType() == dropped -> item -> getType()) {
        attractOther(ITEM_MERGE_DISTANCE, ITEM_ATTRACT_SPEED, dropped);
    }
    /* Now if one of these stacks no longer exists, set location to 
    their average. */
    if (!item || !dropped->item) {
        int avgx = (getRect().x + dropped->getRect().x) / 2;
        int avgy = (getRect().y + dropped->getRect().y) / 2;
        setX(avgx);
        setY(avgy);
        dropped -> setX(avgx);
        dropped -> setY(avgy);
    }
}

void DroppedItem::attract(int x, int y, double xspeed, double yspeed) {
    Movable::attract(x, y, xspeed, yspeed);
    gravity = false;
    collides = false;
    attracting = true;
}

void DroppedItem::update() {
    updateItem();
    if (!attracting || !item) {
        gravity = true;
        collides = true;
    }
    if (throwticks != 0) {
        throwticks--;
    }
    attracting = false;
    setAccel({0, 0});
}


