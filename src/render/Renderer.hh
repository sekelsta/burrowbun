#ifndef RENDERER_HH
#define RENDERER_HH

#include <SDL2/SDL.h>
#include "../Light.hh"
#include <mutex>

/* A class to hold the renderer. This exists because it's probably slightly
better than just making the renderer global. */
class Renderer {
    friend class Texture;
    friend class WindowHandler;
    static std::mutex m;
    static SDL_Renderer *renderer;

public:

    /* Set the render draw color to a light, but with full alpha. */
    inline static void setColor(const Light &color) {
        m.lock();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0xFF);
        m.unlock();
    }

    /* Set the render draw color. */
    inline static void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        m.lock();
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        m.unlock();
    }

    /* Set the render draw color to white. */
    inline static void setColorWhite() {
        m.lock();
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        m.unlock();
    }

    /* Render a rectangle. */
    inline static void renderFillRect(const SDL_Rect &rect) {
        m.lock();
        SDL_RenderFillRect(renderer, &rect);
        m.unlock();
    }

    /* Clear render target with drawing color. */
    inline static void renderClear() {
        m.lock();
        SDL_RenderClear(renderer);
        m.unlock();
    }

    /* Clear render target with drawing color. */
    inline static void setTarget(SDL_Texture *target) {
        m.lock();
        SDL_SetRenderTarget(renderer, target);
        m.unlock();
    }

    /* Put the rendered stuff on the screen. */
    inline static void renderPresent() {
        m.lock();
        SDL_RenderPresent(renderer);
        m.unlock();
    }

};

#endif
