#include <iostream>
#include <vector>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Map.hh"

using namespace std;

// A class to open a window and display things to it
class WindowHandler {
    // Fields
    const int SCREEN_WIDTH;
    const int SCREEN_HEIGHT;

    // The window to render to
    SDL_Window *window;

    // That window's surface
    SDL_Surface *screenSurface;

    // Create a renderer to show textures
    SDL_Renderer *renderer;

    // The path to image files
    const string TILE_PATH;

    // A list of all textures that have been loaded
    vector<SDL_Texture *> textures;

    // A list of all tile textures that have been laoded
    vector<SDL_Texture *> tileTextures;

public:
    // Constructor
    WindowHandler(int width, int height);

    // Start up SDL and open the window
    bool init();

    // Load the images
    bool loadMedia(vector<const Tile *> pointers);

    // Load a texture
    SDL_Texture *loadTexture(const string &name);

    // Load a texture for each tile
    bool loadTiles(vector<const Tile *> pointers);

    // Render everything the map holds information about
    void renderMap(const Map &m);

    // Update the screen
    void update();

    // Clean up and close SDL
    void close();
};

