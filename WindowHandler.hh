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

// Forawrd declare
struct Light;
class Tile;
class Map;
class Movable;
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

using namespace std;

// A struct to hold an SDL_Texture pointer and an SDL_Rect
struct SpriteRect {
    // Fields
    SDL_Texture *texture;
    SDL_Rect rect;

    // Constructor from sprite
    SpriteRect(const Sprite &sprite) {
        texture = sprite.texture;
        rect.w = sprite.width;
        rect.h = sprite.height;
        rect.x = rect.w * sprite.col;
        rect.y = rect.h * sprite.row;
    }

    // Constructor from no arguements
    SpriteRect() {
        texture = NULL;
        rect.x = 0;
        rect.y = 0;
        rect.w = 0;
        rect.h = 0;
    }

    // render itself
    void render(SDL_Renderer *renderer, const SDL_Rect *rectTo) const {
        if (texture != NULL) {
            SDL_RenderCopy(renderer, texture, &rect, rectTo);
        }
    }
};

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

    // Render the texture from the SpriteRect to a 2d grid with width columns
    // and height rows
    void renderGrid(const SpriteRect &sprite, int width, int height);

    // Render a StatBar
    void renderStatBar(StatBar &bar);

    // Create a texture and render all the textures to it, using the spacing 
    // variables from hotbar. The texture to is expected to have the correct 
    // width and height, and the vector is expected to have length 12.
    SDL_Texture *renderHotbarPart(const Hotbar &hotbar, 
        vector<SpriteRect> textures, SDL_Texture *texture) const;

    // Draw the entire hotbar sprite to a texture. This only needs to be called
    // when the hotbar is first made, or when anything about it changes.
    void updateHotbarSprite(Hotbar &hotbar);

    // Render the inventory to the screen
    void renderInventory(Inventory &inventory);

    // Load the images used by all inventories
    bool loadInventory(); //TODO

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
    bool loadMedia(vector<Tile *> &pointers, vector<Movable *> &movables,
        Hotbar &hotbar);

    // Load a texture, return true on success
    bool loadTexture(const string &name);

    // Unload a texture. After calling this function, make sure to set all
    // pointers to the texture to NULL.
    bool unloadTexture(SDL_Texture *texture);

    // Load a texture for each tile
    bool loadTiles(vector<Tile *> &pointers);

    // Load a texture for each movable
    bool loadMovables(vector<Movable *> &movables);

    // Load textures for the hotbar
    bool loadHotbar(Hotbar &hotbar);

    // Load an item or skill sprite
    bool loadAction(Action &action);

    // Render everything the map holds information about
    // x and y are the coordinates of the center of the camera, in pixels,
    // where y = 0 is at the bottom
    void renderMap(const Map &m, const SDL_Rect &camera);

    // Render movables (the player, monsters, NPCs, dropped items)
    void renderMovables(const vector<Movable *> &movables);

    // Update the screen
    void update(const Map &m, const vector<Movable *> &movables, 
        Player &player);

    // Unload media to switch maps, currently done by close()
    void unloadMedia(vector<Tile *> &pointers);

    // Clean up and close SDL
    void close();
};

#endif
