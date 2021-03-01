#include "Sprite.hh"

#include <fstream>
#include <SDL2/SDL_image.h>

/* For convinence. */
using json = nlohmann::json;
using namespace std;

void Sprite::render(const SDL_Rect &rectTo) {
    SpriteBase::render(rect, rectTo);
}

int Sprite::getWidth() const {
    return rect.w;
}

int Sprite::getHeight() const {
    return rect.h;
}

Rect Sprite::getRect() const {
    return rect;
}

void Sprite::renderGrid(int width, int height) {
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = getWidth();
    rectTo.h = getHeight();
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            render(rectTo);
            rectTo.y += rectTo.h;
        }
        rectTo.x += rectTo.w;
        rectTo.y = 0;
    }

}

/* Get a sprite out of a json. */
void from_json(const json &j, Sprite &sprite) {
    /* It's an SDL_Rect so I can't write a from_json for it. */
    sprite = Sprite(j["rect"]["x"], j["rect"]["y"], j["rect"]["w"], 
            j["rect"]["h"], j["name"]);
    
}

