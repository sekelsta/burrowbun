#include "Texture.hh"
#include <SDL2/SDL_image.h>

Texture::Texture(const std::string &name) {
    texture = nullptr;
    assert(Renderer::renderer != nullptr);
    assert(name != "");

    /* Check if a texture with that name has already been loaded. */
    for (unsigned int i = 0; i < loaded.size(); i++) {
        if (loaded[i].name == name) {
            /* Found the texture already loaded, so we copy it, add to
            the reference count, and return. */
            texture = loaded[i].texture;
            loaded[i].count++;
            return;
        }
    }

    /* It wasn't already loaded. */
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

    /* Add it to the list. */
    LoadedTexture newTexture;
    newTexture.name = name;
    newTexture.texture = texture;
    newTexture.count = 1;
    loaded.push_back(newTexture);

}


Texture::Texture(Uint32 pixelFormat, int access, int width, int height) {
    /* This won't be reloaded so there's no need to add it to the list. */
    texture = SDL_CreateTexture(Renderer::renderer, pixelFormat, access, 
            width, height);
}

Texture::~Texture() {
    /* Check if it's in the list of loaded textures. */
    for (unsigned int i = 0; i < loaded.size(); i++) {
        if (loaded[i].texture == texture) {
            loaded[i].count--;
            assert(loaded[i].count >= 0);
            /* If there aren't any textures left, free the memory. */
            if (loaded[i].count == 0) {
                SDL_DestroyTexture(texture);
                loaded.erase(loaded.begin() + i);
                return;
            }
        }
    }

    /* It wasn't in the list, so it should be destroyed. */
    SDL_DestroyTexture(texture);
}

