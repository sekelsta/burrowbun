#include <SDL2/SDL.h>
#include <string>
#include "Light.hh"

/* A wrapper for SDL_Texture. */
class Texture {
    SDL_Texture *texture;
    
public:
    /* Constructor from filename and renderer. */
    Texture(const std::string &name, SDL_Renderer *renderer);
    /* Constructor from all the parameters SDL_CreateTexture() needs. */
    Texture(SDL_Renderer *renderer, Uint32 pixelFormat, int access,
        int width, int height);
    /* Destructor. */
    ~Texture();
    inline void render(SDL_Renderer *renderer, const SDL_Rect *rectFrom, 
            const SDL_Rect *rectTo) const {
        if (texture) {
            SDL_RenderCopy(renderer, texture, rectFrom, rectTo);
        }
    }

    inline int getWidth() {
        int width;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, nullptr);
        return width;
    }

    inline int getHeight() {
        int height;
        SDL_QueryTexture(texture, nullptr, nullptr, nullptr, &height);
        return height;
    }

    inline Uint32 getFormat() {
        Uint32 format;
        SDL_QueryTexture(texture, &format, nullptr, nullptr, nullptr);
        return format;
    }

    /* Functions that mimic the SDL functions. */
    inline void SetTextureColorMod(Light color) {
        if (texture) {
            SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
        }
    }

    inline void SetTextureBlendMode(SDL_BlendMode blendMode) {
        if (texture) {
            SDL_SetTextureBlendMode(texture, blendMode);
        }
    }

    inline void SetRenderTarget(SDL_Renderer *renderer) {
        SDL_SetRenderTarget(renderer, texture);
    }

};
