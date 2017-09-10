#include "Texture.hh"
#include <SDL2/SDL_image.h>

Texture::Texture(const std::string &name) {
    texture = nullptr;
    assert(Renderer::renderer != nullptr);
    /* Load a surface. */
    SDL_Surface *surface = IMG_Load(name.c_str());
    if (surface == nullptr) {
        std::cerr << "Failed to load image with filename " << name << "\n";
        std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
        assert(false);
    }
    /* Make a texture. */
    else {
        // Convert the surface to a texture
        texture = SDL_CreateTextureFromSurface(Renderer::renderer, surface);
        // Get rid of the surface
        SDL_FreeSurface(surface);

        if (texture == nullptr) {
            std::cerr << "Failed to convert surface to texture! Surface loaded";
            std::cerr << " from " << name << "\nSDL_Error: " << SDL_GetError();
            std::cerr << "\n";
            assert(false);
        }
    }

}


Texture::Texture(Uint32 pixelFormat, int access, int width, int height) {
    texture = SDL_CreateTexture(Renderer::renderer, pixelFormat, access, 
            width, height);
}

Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

