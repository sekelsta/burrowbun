#ifndef RENDERER_HH
#define RENDERER_HH

#include <SDL2/SDL.h>

/* A class to hold the renderer. This exists because it's probably slightly
better than just making the renderer global. */
class Renderer {
public:
    static SDL_Renderer *renderer;
};

#endif
