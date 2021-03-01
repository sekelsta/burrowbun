#ifndef TEXTURE_HH
#define TEXTURE_HH

#include "../Light.hh"
#include "Renderer.hh"

#include <iostream>
#include <mutex>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

#define FONT_NAME "FreeMonoBold.ttf"
#define DEFAULT_OUTLINE_SIZE 1

/* A struct for holding an SDL_Texture * and filename so that the same texture
doesn't get loaded twice, as well as a count of how many Textures are using 
it. */
struct LoadedTexture {
    SDL_Texture *texture;
    std::string name;
    int count;
};

struct LoadedFont {
    TTF_Font *font;
    std::string name;
    int size;
    int outline; // nonzero iff font is an outline
};

/* A wrapper for SDL_Texture. Also avoids loading multiple copies of the same
texture, and destroys textures when it's time. */
class Texture {
    SDL_Texture *texture;

    /* For multithreaded access to static variables. */
    static std::mutex m;

    /* For keeping track of which textures have been loaded. */
    static std::vector<LoadedTexture> loaded;

    /* For keeping track of which fonts have been loaded. */
    static std::vector<LoadedFont> fonts;

    /* Render text to a texture, with proper wrapping, and an outline. */
    SDL_Texture *getText(std::string text, int size, 
        int outline_size, Light color, Light outline_color, int wrap_length);

    /* Return a font with the specified characteristics. */
    static TTF_Font *getFont(std::string name, int size, int outline);

    /* Add the texture to the list of loaded textures, or increase the count
    if it's already there. */
    void addToLoaded();

public:
    /* Constructor from filename of the picture. */
    Texture(const std::string &name);

    /* Constructor with text to display and font size.
    By default renders white text with a black outline. */
    Texture(std::string text, int size, int wrap_length);
    Texture(std::string text, int size, int outline_size,
        Light color, Light outline_color, int wrap_length);

    /* Constructor from all the parameters SDL_CreateTexture() needs (except
    the renderer, which is a global variable). */
    Texture(Uint32 pixelFormat, int access, int width, int height);

    /* Copy constructor. */
    Texture(const Texture &other);

    /* Non-renderable texture. */
    inline Texture() {
        texture = nullptr;
    }

    inline bool hasTexture()  const{
        return (bool)texture;
    }

    /* Operator= */
    Texture &operator=(const Texture &other);

    /* Destructor. */
    ~Texture();

    /* Get the path to the executable. */
    static std::string getPath();

    /* Render itself. */
    inline void render(const SDL_Rect &rectFrom, const SDL_Rect &rectTo) const {
        if (texture) {
            assert(rectFrom.w != 0);
            assert(rectFrom.h != 0);
            assert(rectTo.w != 0);
            assert(rectTo.h != 0);
            assert(rectFrom.x >= 0);
            assert(rectFrom.y >= 0);
            /* rectTo can have x or y less than 0, that just means it'll
            be rendered a bit off the screen. */
            m.lock();
            Renderer::m.lock();
            SDL_RenderCopy(Renderer::renderer, texture, &rectFrom, &rectTo);
            Renderer::m.unlock();
            m.unlock();
        }
    }

    /* Render the whole image at normal size with the top-left corner at x, y */
    inline void render(int x, int y) const {
        if (texture) {
            /* rectTo can have x or y less than 0, that just means it'll
            be rendered a bit off the screen. */
            SDL_Rect rectTo = {x, y, getWidth(), getHeight()};
            assert(rectTo.w != 0);
            assert(rectTo.h != 0);
            Renderer::m.lock();
            SDL_RenderCopy(Renderer::renderer, texture, nullptr, &rectTo);
            Renderer::m.unlock();
        }
    }

    inline int getWidth() const {
        assert(texture);
        int width;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, nullptr);
        return width;
    }

    inline int getHeight() const {
        assert(texture);
        int height;
        SDL_QueryTexture(texture, nullptr, nullptr, nullptr, &height);
        return height;
    }

    inline Uint32 getFormat() const {
        assert(texture);
        Uint32 format;
        SDL_QueryTexture(texture, &format, nullptr, nullptr, nullptr);
        return format;
    }

private:
    /* Wrapper for TTF_Size_Text. */
    static inline void SizeText(int size, std::string s, int *w, int *h, 
            int outline) {
        int success = TTF_SizeText(getFont(FONT_NAME, size, outline), 
            s.c_str(), w, h);
        if (success != 0) {
            std::string message = (std::string)"The font " + FONT_NAME 
                + " does not have " + "a glyph in " + s + "\n";
            std::cerr << message;
            throw message;
        }
    }
public:
    static inline int getTextWidth(int size, std::string s, int outline) {
        int w;
        SizeText(size, s, &w, nullptr, outline);
        return w;
    }

    static inline int getTextHeight(int size, std::string s, int outline) {
        int h;
        SizeText(size, s, nullptr, &h, outline);
        return h;
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

    inline void SetTextureAlphaMod(Uint8 a) {
        if (texture) {
            int success = SDL_SetTextureAlphaMod(texture, a);
            if (success < 0) {
                std::cerr << "Warning: renderer does not support alpha "
                        << "modulation.\n";
            }
        }
    }

    inline void SetRenderTarget() {
        m.lock();
        Renderer::setTarget(texture);
        m.unlock();
    }

    static inline void closeFonts() {
        m.lock();
        for (unsigned int i = 0; i < fonts.size(); i++) {
            TTF_CloseFont(fonts[i].font);
        }
        m.unlock();
    }

};

#endif
