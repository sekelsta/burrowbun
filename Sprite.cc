#include <fstream>
#include <SDL2/SDL_image.h>
#include "Sprite.hh"
#include "json.hpp"

/* For convinence. */
using json = nlohmann::json;
using namespace std;

// Constructor
Sprite::Sprite() {
    // Initialize all the values
    name = "";
    rect.x = 0;
    rect.y = 0;
    rect.w = 0;
    rect.h = 0;
}

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

void Sprite::loadTexture(std::string prefix, SDL_Renderer *renderer) {
    if (name != "") {
        texture.reset(new Texture(prefix + name, renderer));
    }
}

/* Get a sprite out of a json. */
void from_json(const json &j, Sprite &sprite) {
    /* Set each of this tile's non-const values equal to the json's values. */
    sprite.name = j["name"];
    /* It's an SDL_Rect so I can't write a from_json for it. */
    sprite.rect.w = j["rect"]["w"];
    sprite.rect.h = j["rect"]["h"];
    sprite.rect.x = j["rect"]["x"];
    sprite.rect.y = j["rect"]["y"];
    
}

