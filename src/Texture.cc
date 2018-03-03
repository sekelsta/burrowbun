#include "Texture.hh"
#include "filepaths.hh"
#include "Game.hh"
#include <SDL2/SDL_image.h>

#define DEFAULT_OUTLINE_COLOR {0x00, 0x00, 0x00, 0xFF}
#define DEFAULT_TEXT_COLOR {0xFF, 0xFF, 0xFF, 0xFF}

using namespace std;

/* Declare static variables. */
std::vector<LoadedTexture> Texture::loaded;
std::vector<LoadedFont> Texture::fonts;


SDL_Texture *Texture::getText(string text, string path, int size, 
        int outline_size, Light color, Light outline_color, int wrap_length) {
    TTF_Font *font = getFont(FONT_NAME, size, 0);
    TTF_Font *font_outline = getFont(FONT_NAME, size, outline_size);
    
    // Render text surface
    SDL_Surface *bg_surface = nullptr;
    SDL_Surface *fg_surface = nullptr;
    if (wrap_length) {
        bg_surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, 
            wrap_length);
    }
    else {
        bg_surface = TTF_RenderText_Blended(font_outline, 
            text.c_str(), outline_color);
        fg_surface = TTF_RenderText_Blended(font, text.c_str(), color);
    }
    if ((!fg_surface && !wrap_length) || !bg_surface) {
        string message = (string)"Unable to render text to surface!\n"
            + "SDL_ttf error: " + TTF_GetError() + "\n";
        cerr << message;
        throw message;
    }

    if (!wrap_length) {
        SDL_Rect rect = {outline_size, outline_size, fg_surface->w, 
            fg_surface->h};
        /* blit text onto its outline */ 
        SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND); 
        SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect); 
        SDL_FreeSurface(fg_surface); 
    }

    // Create and return texture from surface
    SDL_Texture *answer = SDL_CreateTextureFromSurface(Renderer::renderer, 
        bg_surface);
    SDL_FreeSurface(bg_surface);
    return answer;   
}

TTF_Font *Texture::getFont(string name, int size, int outline) {
    for (unsigned int i = 0; i < fonts.size(); i++) {
        if (fonts[i].name == name && fonts[i].size == size 
                && fonts[i].outline == outline) {
            return fonts[i].font;
        }
    }
    /* Font not found. Try loading one. */
    string fontfile = getPath() + FONT_FILE_PATH + name;
    TTF_Font *font = TTF_OpenFont((fontfile).c_str(), size);
    if (outline) {
        TTF_SetFontOutline(font, outline);
    }
    if (!font) {
        string message = (string)"Failed to load font " + fontfile
            + ".\nSDL_ttf Error: " + TTF_GetError() + "\n";
        cerr << message;
        throw message;
    }
    
    fonts.push_back({font, name, size, outline});
    return font;
}

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
        string message = (string)"Failed to load image with filename " + name 
        + (string)"\nSDL_Error: " + SDL_GetError() + "\n";
        cerr << message;
        throw message;
    }
    /* Make a texture. */
    else {
        // Convert the surface to a texture
        texture = SDL_CreateTextureFromSurface(Renderer::renderer, surface);
        // Get rid of the surface
        SDL_FreeSurface(surface);

        if (texture == nullptr) {
            string message = (string)"Failed to convert surface to texture!"
                + (string)" Surface loaded from " + name + "\nSDL_Error: " 
                + SDL_GetError() + "\n";
            cerr << message;
            throw message;
        }
    }

    /* Add it to the list. */
    LoadedTexture newTexture;
    newTexture.name = name;
    newTexture.texture = texture;
    newTexture.count = 1;
    loaded.push_back(newTexture);

}

Texture::Texture(string text, string path, int size, int wrap_length) 
    : Texture(text, path, size, DEFAULT_OUTLINE_SIZE, DEFAULT_TEXT_COLOR,
        DEFAULT_OUTLINE_COLOR, wrap_length) {}

Texture::Texture(string text, string path, int size, int outline_size,
        Light color, Light outline_color, int wrap_length) {
    texture = getText(text, path, size, outline_size, color, 
        outline_color, wrap_length);
}

Texture::Texture(Uint32 pixelFormat, int access, int width, int height) {
    /* This won't be reloaded so there's no need to add it to the list. */
    texture = SDL_CreateTexture(Renderer::renderer, pixelFormat, access, 
            width, height);
    /* Draw alpha to the texture while we're at it. */
    SetTextureBlendMode(SDL_BLENDMODE_BLEND);
    SetRenderTarget();
    /* Set render draw color to alpha. */
    Renderer::setColor({0x00, 0x00, 0x00, 0x00});
    SDL_RenderClear(Renderer::renderer);
    /* Set render color back to white. */
    Renderer::setColorWhite();
    /* And stop drawing to the texture. */
    SDL_SetRenderTarget(Renderer::renderer, NULL);
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
            }
            return;
        }
    }

    /* It wasn't in the list, so it should be destroyed. */
    SDL_DestroyTexture(texture);
}

string Texture::getPath() {
    return Game::getPath();
}



