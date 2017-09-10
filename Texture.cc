#include "Texture.hh"
#include <SDL2/SDL_image.h>

Texture::Texture(const std::string &name, SDL_Renderer *renderer) {
    texture = nullptr;
    /* Load a surface. */
    SDL_Surface *surface = IMG_Load(name.c_str());
    if (surface == nullptr) {
        throw "Failed to load image with filename " + name + "\n"
                + "SDL_Error: " + SDL_GetError() + "\n";
    }
    /* Make a texture. */
    else {
        // Convert the surface to a texture
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        // Get rid of the surface
        SDL_FreeSurface(surface);

        if (texture == nullptr) {
            throw "Failed to convert surface to texture! Surface loaded from "
                    + name + "\nSDL_Error: " + SDL_GetError() + "\n";
        }
    }

}


Texture::Texture(SDL_Renderer *renderer, Uint32 pixelFormat, int access,
        int width, int height) {
    texture = SDL_CreateTexture(renderer, pixelFormat, access, width, height);
}

Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

