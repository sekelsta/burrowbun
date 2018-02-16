#include "DroppedItem.hh"

using namespace std;

DroppedItem::DroppedItem(Item *i, int x, int y) {
    item = i;
    setX(x);
    setY(y);
    rect.w = i->sprite.getWidth();
    rect.h = i->sprite.getHeight();
}

DroppedItem::~DroppedItem() {
    delete item;
}

void DroppedItem::render(const Rect &camera) {
    // Make sure the renderer draw color is set to white
    Renderer::setColorWhite();

    SDL_Rect to = {rect.x, rect.y, rect.w, rect.h};
    convertRect(to, camera);
    item -> sprite.render(to);
}


