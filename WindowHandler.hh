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

#include "Sprite.hh"
#include "Movable.hh"
#include "UIHelpers.hh"

// Forawrd declare
struct Light;
class Tile;
class Map;
class Hotbar;
class Player;
class Inventory;
class Action;
struct MouseBox;
struct StatBar;

// The path to image files
#define TILE_PATH "content/Blocks/"
#define MOVABLE_PATH "content/"
#define UI_PATH "content/"
#define ICON_PATH "content/Icons/"

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

    // A 2D vector of SLD rects for rendering the map
    std::vector<std::vector<SDL_Rect>> tileRects;

    // Store the texture to draw over the player's stat bars
    Sprite statBarOverlay; // TODO: make this a static member of StatBar

    // Private methods

    // Return a rectangle in world coordinates, for a player at x, y
    // w and h are the width and height of the player sprite.
    SDL_Rect findCamera(int x, int y, int w, int h);

    // Convert a rectangle from world coordinates to screen coordinates
    SDL_Rect convertRect(SDL_Rect rect, SDL_Rect camera);

    // Set render draw color to light color
    void setRenderColorToLight(const Light &color);

    // Render the texture from the Sprite to a 2d grid with width columns
    // and height rows
    void renderGrid(const Sprite &sprite, int width, int height);

    // Render a StatBar
    void renderStatBar(StatBar &bar);

    // Create a texture and render all the textures to it, using the spacing 
    // variables from hotbar. The texture to is expected to have the correct 
    // width and height, and the vector is expected to have length 12.
    SDL_Texture *renderHotbarPart(const Hotbar &hotbar, 
        std::vector<Sprite> textures, SDL_Texture *texture) const;

    // Draw the entire hotbar sprite to a texture. This only needs to be called
    // when the hotbar is first made, or when anything about it changes.
    void updateHotbarSprite(Hotbar &hotbar);

    // Render the inventory to the screen
    void renderInventory(Inventory &inventory);

    // Load the images used by all inventories
    void loadInventory();

    /* Draw the whole inventory onto a single sprite. */
    void updateInventorySprite(Inventory &inventory);

    // Render everything UI
    void renderUI(Player &player);

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
    void loadMedia(std::vector<Tile *> &pointers, 
        std::vector<movable::Movable *> &movables, Hotbar &hotbar);

    // Load an item or skill sprite
    void loadAction(Action &action);

    // Render everything the map holds information about
    // x and y are the coordinates of the center of the camera, in pixels,
    // where y = 0 is at the bottom
    void renderMap(Map &m, const SDL_Rect &camera);

    // Render movables (the player, monsters, NPCs, dropped items)
    void renderMovables(const std::vector<movable::Movable *> &movables);

    // Update the screen
    void update(Map &m, const std::vector<movable::Movable *> &movables, 
        Player &player);

    // Unload media to switch maps, currently done by close()
    void unloadMedia(std::vector<Tile *> &pointers);

    // Clean up and close SDL
    void close();
};

#endif
