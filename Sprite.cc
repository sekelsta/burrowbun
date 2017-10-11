#include <fstream>
#include <SDL2/SDL_image.h>
#include "Sprite.hh"
#include "json.hpp"

/* For convinence. */
using json = nlohmann::json;
using namespace std;

/* Assignment operator. */
Sprite &Sprite::operator=(const Sprite &sprite) {
    /* Check for self-assignment. */
    if (this == &sprite) {
        return *this;
    }
    name = sprite.name;
    texture = sprite.texture;
    rect = sprite.rect;

    return *this;
}

void Sprite::render(const SDL_Rect &rectTo) {
    SpriteBase::render(rect, rectTo);
}

int Sprite::getWidth() const {
    return rect.w;
}

int Sprite::getHeight() const {
    return rect.h;
}

/* Get a sprite out of a json. */
void from_json(const json &j, Sprite &sprite) {
    /* It's an SDL_Rect so I can't write a from_json for it. */
    sprite = Sprite(j["rect"]["x"], j["rect"]["y"], j["rect"]["w"], 
            j["rect"]["h"], j["name"]);
    
}

