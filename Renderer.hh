#ifndef RENDERER_HH
#define RENDERER_HH

#include <SDL2/SDL.h>
#include "Light.hh"

/* A class to hold the renderer. This exists because it's probably slightly
better than just making the renderer global. */
class Renderer {
public:
    static SDL_Renderer *renderer;

    /* Set the render draw color to a light, but with full alpha. */
    inline static void setColor(const Light &color) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0xFF);
    }

    /* Set the render draw color to white. */
    inline static void setColorWhite() {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
};

#endif
