#ifndef SPRITE_HH
#define SPRITE_HH

#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include "Texture.hh"
#include "json.hpp"

// Forward declare
struct SDL_Texture;

struct Sprite {
    /* The name of the sprite image. This may be a spritesheet. */
    std::string name;
    std::shared_ptr<Texture> texture;

    /* Rectangle of where to render from. */
    SDL_Rect rect;

    inline int getWidth() const {
        assert(hasTexture());
        return texture -> getWidth();
    }

    /* Returns the number of columns spriteWidth apart this spritesheet can 
    hold. */
    inline int getCols() const {
        return getWidth() / rect.w;
    }

    inline int getHeight() const {
        assert(hasTexture());
        return texture -> getHeight();
    }

    /* Returns the number of rows of spriteHeight this spritesheet can hold. */
    inline int getRows() const {
        return getHeight() / rect.h;
    }

    inline bool hasTexture() const {
        return (bool)texture;
    }

    /* The constructor. */
    Sprite();

    /* Assignment operator. */
    Sprite &operator=(const Sprite &sprite);

    /* Load an SDL texture. */
    void loadTexture(std::string prefix);

    /* Render itself. */
    inline void render(const SDL_Rect *rectTo) const {
        if (hasTexture()) {
            texture -> render(&rect, rectTo);
        }
        else {
            /* It should be loaded before being rendered, and the only way it
            can be loaded properly without having a sprite is if name == "". */
            assert(name == "");
        }
    }
};

/* Get a sprite from a json. */
void from_json(const nlohmann::json &j, Sprite &sprite);

#endif
