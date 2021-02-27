#ifndef SPRITEBASE_HH
#define SPRITEBASE_HH

#include "Texture.hh"
#include "../Light.hh"
#include "../Rect.hh"

/* Class that has a texture and knows the size. */
class SpriteBase {
protected:
    Light color;

    /* Render, using the color. Public render functions should call this. */
    inline void render(const SDL_Rect &rectFrom, const SDL_Rect &rectTo) {
        if (hasTexture()) {
            texture -> SetTextureColorMod(color);
            texture -> SetTextureAlphaMod(color.a);
            texture -> render(rectFrom, rectTo);
        }
        else {
            /* It should be loaded before being rendered, and the only way it
            can be loaded without having a texture is if name == "". */
            assert(name == "");
        }
    }
public:
    /* Name of the texture, and texture. Can be a spritesheet. */
    std::string name;
    std::shared_ptr<Texture> texture;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual Rect getRect() const = 0;
    /* Return true if the texture pointer is not null. */
    inline bool hasTexture() const {
        return (bool)texture;
    }

    /* Return the width and height of the stored texture. */
    inline int getTextureWidth() const {
        assert(hasTexture());
        return texture -> getWidth();
    }
    inline int getTextureHeight() const {
        assert(hasTexture());
        return texture -> getHeight();
    }


    virtual void render(const SDL_Rect &rectTo) = 0;

    /* Constructor. */
    inline SpriteBase() {
        color = {0xFF, 0xFF, 0xFF, 0xFF};
    }

    /* Virtual destructor. */
    virtual ~SpriteBase();

    /* Load the texture. */
    inline void loadTexture(std::string prefix) {
        if (name != "") {
            texture.reset(new Texture(prefix + name));
        }
    }
};

#endif
