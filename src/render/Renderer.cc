#include "Renderer.hh"

/* Initialize the static value. */
std::mutex Renderer::m;
SDL_Renderer *Renderer::renderer = NULL;
