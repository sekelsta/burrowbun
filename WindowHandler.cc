#include "WindowHandler.hh"
using namespace std;

// Constructor
WindowHandler::WindowHandler(int width, int height) 
    : SCREEN_WIDTH(width), SCREEN_HEIGHT(height), TILE_PATH("content/") {
    window = NULL;
    screenSurface = NULL;
    renderer = NULL;
}

// Start up the window
bool WindowHandler::init() {
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize. SDL_Error: ";
        cerr << SDL_GetError() << endl;
        success = false;
    }
    else {
        // Create window
        window = SDL_CreateWindow("Hello whirreled!", SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                    SDL_WINDOW_SHOWN);
        if (window == NULL) {
            cerr << "Window could not be created. SDL_Error: ";
            cerr << SDL_GetError() << endl;
            success = false;
        }
        else {
            // Create a renderer for the window
            renderer = SDL_CreateRenderer(window, -1, 
                                            SDL_RENDERER_ACCELERATED);

            // Fall back to a software renderer if necessary
            if (renderer == NULL) {
                cerr << "Hardware-accelerated renderer could not be created. ";
                cerr << "SDL Error: " << endl << SDL_GetError() << endl;
                cerr << "Falling back to software renderer." << endl;
                renderer = SDL_CreateRenderer(window, -1, 
                                            SDL_RENDERER_SOFTWARE);
            }
            if (renderer == NULL) {
                cerr << "Software-accelerated renderer could not be created. ";
                cerr << "SDL Error: " << endl << SDL_GetError() << endl;
                success = false;
            }
            else {
                // Initialize renderer draw color
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    cerr << "SDL_image could not initialize. SDL_image Error: ";
                    cerr << endl << IMG_GetError() << endl;
                    success = false;
                }
            }
        }
    }
    return success;
}

// Load test.bmp to surface, return true if successful
bool WindowHandler::loadMedia(vector<const Tile *> pointers) {
    bool success = true;

    // Load the textures for tiles
    loadTiles(pointers);

    return success;
}

// Load an image onto a surface and convert it to match the screen
SDL_Texture *WindowHandler::loadTexture(const string &name) {
    // Declare variables and load a surface
    SDL_Surface *surface = IMG_Load(name.c_str());
    SDL_Texture *texture = NULL;
    if (surface == NULL) {
        cerr << "Unable to load image " << name << endl;
        cerr << "SDLError: " << SDL_GetError() << endl;
    }
    // Make a texture
    else {
        // Convert the surface to a texture
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == NULL) {
            cerr << "Failed to crate texture from " <<  name << endl;
            cerr << "SDL Error: " << SDL_GetError() << endl;
        }
        // Get rid of the surface
        SDL_FreeSurface(surface);
    }

    // Add it to the list of textures so it can be deallocated later
    textures.push_back(texture);

    return texture;
}

// Load a texture for each tile
bool WindowHandler::loadTiles(vector<const Tile *> pointers) {
    bool success = true;
    for (unsigned i = 0; i < pointers.size(); i++) {
        loadTexture(TILE_PATH + (pointers[i] -> sprite));
        assert(textures.size() != 0);
        if (textures.back() == NULL) {
            success = false;
        }
        // Add the texture to the list of tile textures
        tileTextures.push_back(textures.back());
    }

    return success;
}

// Render everything the map holds information about
void WindowHandler::renderMap(const Map &m) {
    // TODO
}

// Update the screen
void WindowHandler::update() {
    // Clear the screen
    SDL_RenderClear(renderer);

    // Put stuff on it
    SDL_Rect stretch = { 0, 0, 20, 20 };
    SDL_RenderCopy(renderer, textures.back(), NULL, NULL);
    SDL_RenderCopy(renderer, textures.back(), NULL, &stretch);

    // Update the screen
    SDL_RenderPresent(renderer);
}

// Close the window, clean up, and exit SDL
void WindowHandler::close() {
    // Deallocate any textures
    for(unsigned i = 0; i < textures.size(); i++) {
        // If I were being a good programmer I'd make the pointers
        // point to NULL
        SDL_DestroyTexture(textures[i]);
    }

    // Destroy window
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}
