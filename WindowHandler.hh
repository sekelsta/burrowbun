#ifndef WINDOWHANDLER_HH
#define WINDOWHANDLER_HH

/* Note: There are three coordinate systems in use. Tile coordinates measure
in tiles, and have y = 0 at the bottom of the map. World coordinates measure
in pixels, and also have y = 0 at the bottom of the map. Screen coordinates
also measure in pixels, but have y = 0 at the top of the screen. Rendering is
the only thing screen coordinates are used for. */

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include "Light.hh"
#include "Tile.hh"
#include "Map.hh"
#include "Movable.hh"

// The path to image files
#define TILE_PATH "content/"
#define MOVABLE_PATH "content/"

using namespace std;

// A class to open a window and display things to it
class WindowHandler {
    // Fields
    int screenWidth;
    int screenHeight;

    // Height and width of tiles
    const int TILE_WIDTH;
    const int TILE_HEIGHT;

    // Height and width of the world, in pixels
    // Not const because it should eventually be possible to switch maps
    int worldWidth;
    int worldHeight;

    // Whether to use the tile's light when rendering
    bool enableDarkness;

    // Whether the window is minimized
    bool isMinimized;

    // The window to render to
    SDL_Window *window;

    // That window's surface
    SDL_Surface *screenSurface;

    // Create a renderer to show textures
    SDL_Renderer *renderer;

    // A list of all textures that have been loaded
    vector<SDL_Texture *> textures;

    // A 2D vector of SLD rects for rendering the map
    vector<vector<SDL_Rect>> tileRects;

    // Private methods

    // Return a rectangle in world coordinates, for a player at x, y
    // w and h are the width and height of the player sprite.
    SDL_Rect findCamera(int x, int y, int w, int h);

    // Convert a rectangle from world coordinates to screen coordinates
    SDL_Rect convertRect(SDL_Rect rect, SDL_Rect camera);

public:
    // Constructor
    WindowHandler(int screenWidth, int screenHeight, int mapWidth, 
        int mapHeight, int tileWidth, int tileHeight, bool dark);

    // Access functions
    void setMinimized(bool minimized);
    void resize(int width, int height);

    // Start up SDL and open the window
    bool init();

    // Load the images
    bool loadMedia(vector<Tile *> &pointers, vector<Movable *> &movables);

    // Load a texture
    SDL_Texture *loadTexture(const string &name);

    // Load a texture for each tile
    bool loadTiles(vector<Tile *> &pointers);

    // Load a texture for each movable
    bool loadMovables(vector<Movable *> &movables);

    // Render everything the map holds information about
    // x and y are the coordinates of the center of the camera, in pixels,
    // where y = 0 is at the bottom
    void renderMap(const Map &m, const SDL_Rect &camera);

    // Render movables (the player, monsters, NPCs, dropped items)
    void renderMovables(const vector<Movable *> &movables);

    // Update the screen
    void update(const Map &m, const vector<Movable *> &movables);

    // Unload media to switch maps, currently done by close()
    void unloadMedia(vector<Tile *> &pointers);

    // Clean up and close SDL
    void close();
};

#endif
