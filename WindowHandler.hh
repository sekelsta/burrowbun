/* Note: There are three coordinate systems in use. Tile coordinates measure
in tiles, and have y = 0 at the bottom of the map. World coordinates measure
in pixels, and also have y = 0 at the bottom of the map. Screen coordinates
also measure in pixels, but have y = 0 at the top of the screen. Rendering is
the only thing screen coordinates are used for. */

#include <assert.h>
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

    // Height and width of tiles
    const int TILE_WIDTH;
    const int TILE_HEIGHT;

    // The path to image files
    const string TILE_PATH;

    // Height and width of the world, in pixels
    // Not const because it should eventually be possible to switch maps
    int worldWidth;
    int worldHeight;

    // The window to render to
    SDL_Window *window;

    // That window's surface
    SDL_Surface *screenSurface;

    // Create a renderer to show textures
    SDL_Renderer *renderer;

    // A list of all textures that have been loaded
    vector<SDL_Texture *> textures;

    // A 3D vector of SLD rects for rendering the map
    vector<vector<vector<SDL_Rect>>> tileRects;

    // Private methods

    // Return a rectangle in world coordinates, with x and y at the center
    SDL_Rect findCamera(int x, int y);

    // Convert a rectangle from world coordinates to screen coordinates
    SDL_Rect convertRect(SDL_Rect rect, int x, int y);

public:
    // Constructor
    WindowHandler(int screenWidth, int screenHeight, int mapWidth, 
                    int mapHeight);

    // Start up SDL and open the window
    bool init();

    // Load the images
    bool loadMedia(vector<Tile *> &pointers);

    // Load a texture, color keying rgb to transparent
    SDL_Texture *loadTexture(const string &name, Uint8 r, Uint8 g, Uint8 b);

    // Load a texture for each tile
    bool loadTiles(vector<Tile *> &pointers);

    // Render everything the map holds information about
    // x and y are the coordinates of the center of the camera, in pixels,
    // where y = 0 is at the bottom
    void renderMap(const Map &m, unsigned x, unsigned y);

    // Update the screen
    void update(const Map &m);

    // Unload media to switch maps, currently done by close()
    void unloadMedia(vector<Tile *> &pointers);

    // Clean up and close SDL
    void close();
};

