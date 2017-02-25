#include <cassert>
#include <iostream>
#include <SDL2/SDL_image.h>
#include "WindowHandler.hh"

using namespace std;

// Return a rectangle in world coordinates for a player at x, y
// w and h are the width and height of the player sprite
SDL_Rect WindowHandler::findCamera(int x, int y, int w, int h) {
    SDL_Rect camera;
    camera.x = x - screenWidth / 2;
    camera.y = y - screenHeight / 2;
    camera.w = screenWidth;
    camera.h = screenHeight;

    // Make the camera center be the center of the player
    camera.x += w / 2;
    camera.y += h / 2;

    // Adjust the camera if necessary, so that it's entirely on the screen
    // Because modulo can return a negative
    camera.x += worldWidth;
    camera.x %= worldWidth;
    camera.y = min(camera.y, worldHeight - screenHeight - TILE_HEIGHT);
    camera.y = max(0, camera.y);

    // If, God forbid, the map is smaller than the camera, shrink the camera
    camera.w = min(camera.w, worldWidth - TILE_HEIGHT);
    camera.h = min(camera.h, worldHeight - TILE_WIDTH);

    return camera;
}

// Convert a rectangle from world coordinates to screen coordinates
SDL_Rect WindowHandler::convertRect(SDL_Rect rect, SDL_Rect camera) {
    int xScreen = (rect.x - camera.x + worldWidth) % worldWidth;
    int yScreen = camera.y + camera.h - rect.y - rect.h;
    SDL_Rect screenRect = { xScreen, yScreen, rect.w, rect.h };

    return screenRect;
}

// Constructor
WindowHandler::WindowHandler(int screenWidth, int screenHeight, 
        int mapWidth, int mapHeight, int tileWidth, int tileHeight, bool dark) 
    : screenWidth(screenWidth), screenHeight(screenHeight), 
        TILE_WIDTH(tileWidth), TILE_HEIGHT(tileHeight), 
        worldWidth(TILE_WIDTH * mapWidth),
        worldHeight(TILE_HEIGHT * mapHeight), enableDarkness(dark) {
    window = NULL;
    screenSurface = NULL;
    renderer = NULL;

    // Set the 2D vector of rects for the tiles
    resize(screenWidth, screenHeight);
}

void WindowHandler::setMinimized(bool minimized) {
    isMinimized = minimized;
}

void WindowHandler::resize(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    // Set the 2D vector of rects for the tiles
    // Access goes [x][y]
    tileRects.resize(ceil((float)screenWidth / (float)TILE_WIDTH) + 1);
    for (unsigned i = 0; i < tileRects.size(); i++) {
        tileRects[i].resize(ceil((float)screenHeight / (float)TILE_HEIGHT) + 1);
        // Set the tileRects to have the correct width and height
        for (unsigned j = 0; j < tileRects[i].size(); j++) {
            tileRects[i][j].w = TILE_WIDTH;
            tileRects[i][j].h = TILE_HEIGHT;
        }
    }
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
                    SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight,
                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
    // Assume the window is not minmized
    isMinimized = false;

    return success;
}

// Load test.bmp to surface, return true if successful
bool WindowHandler::loadMedia(vector<Tile *> &pointers, 
        vector<Movable *> &movables) {
    bool success = true;

    // Load the textures for tiles
    success = loadTiles(pointers);

    // And for the movables
    success = success && loadMovables(movables);

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
bool WindowHandler::loadTiles(vector<Tile *> &pointers) {
    bool success = true;
    for (unsigned i = 0; i < pointers.size(); i++) {
        string name = TILE_PATH + pointers[i] -> sprite;
        if (name != TILE_PATH) {
            loadTexture(name);
            assert(textures.size() != 0);
            if (textures.back() == NULL) {
                success = false;
            }

            // Set the tile's texture to the loaded texture
            assert(pointers[i] -> texture == NULL);
            pointers[i] -> texture = textures.back();
        }
    }

    return success;
}

// Load a texture for each movable
bool WindowHandler::loadMovables(vector<Movable *> &movables) {
    bool success = true;
    for (unsigned int i = 0; i < movables.size(); i++) {
        string name = MOVABLE_PATH + movables[i] -> getSprite();
        loadTexture(name);
        assert(textures.size() != 0);
        if (textures.back() == NULL) {
            success = false;
        }

        movables[i] -> texture = textures.back();
    }

    return success;
}

// Render everything the map holds information about
// x and y are the center of view of the camera, in pixels, 
// where y = 0 at the bottom
// If either value puts the camera past the end of the map, it will be fixed
void WindowHandler::renderMap(const Map &m, const SDL_Rect &camera) {
    // Make sure the renerer draw color is set to white
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    assert(camera.x >= 0);
    assert(camera.y >= 0);

    // Rectangle to draw to
    SDL_Rect *rectTo;
    // Rectangle to draw from, to pick part of a spritesheet
    SDL_Rect rectFrom;
    rectFrom.w = TILE_WIDTH;
    rectFrom.h = TILE_HEIGHT;

    // Iterate through every tile at least partially within the camera
    int width = ceil((float)camera.w / (float)TILE_WIDTH) + 1;
    int height = ceil((float)camera.h / (float)TILE_HEIGHT) + 1;
    int mapWidth = worldWidth / TILE_WIDTH;
    int xMapStart = ((camera.x / TILE_WIDTH) + mapWidth) % mapWidth;
    int yMapStart = (camera.y + camera.h) / TILE_HEIGHT;
    for (int i = 0; i < width; i++) {
        int xRectTo = i * TILE_WIDTH - (camera.x % TILE_WIDTH);
        for (int j = 0; j < height; j++) {
            // Set the rectangle to the correct one in the vector2D
            rectTo = &tileRects[i][j];

            rectTo -> x = xRectTo;

            // Remember that screen y == 0 at the top but world y == 0 at 
            // the bottom. Here j == 0 at the top of the screen.
            // We're not using convertRect because that doesn't align them
            // with the tile grid.
            rectTo -> y = ((camera.h + camera.y) % TILE_HEIGHT) - TILE_HEIGHT;
            rectTo -> y += j * TILE_HEIGHT;

            // Render the tile
            int xTile = (xMapStart + i) % mapWidth;
            int yTile = yMapStart - j;
            // But only if it's a tile that exists on the map
            assert (0 <= xTile && xTile < mapWidth && 0 <= yTile 
                && yTile < worldHeight / TILE_HEIGHT);
            Tile *tile = m.getForeground(xTile, yTile);
            if (tile -> texture != NULL) {
                // Modulate the color due to lighting
                Light light = m.getLight(xTile, yTile);
                // Only add darkness if darkness is enabled
                if(enableDarkness) {
                    SDL_SetTextureColorMod(tile -> texture, light.r, 
                        light.g, light.b);
                }
                rectFrom.x = m.getSpritePlace(xTile, yTile).x * TILE_WIDTH;
                rectFrom.y = m.getSpritePlace(xTile, yTile).y * TILE_HEIGHT;                    SDL_RenderCopy(renderer, tile -> texture, &rectFrom, rectTo);
            }
        }
    }
}

// Render any movables (the player, monsters NPCs, dropped items)
void WindowHandler::renderMovables(const vector<Movable *> &movables) {
    // Make sure the renerer draw color is set to white
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Find where the player and camera are
    int x = movables[0] -> x;
    int y = movables[0] -> y;
    int w = movables[0] -> spriteWidth;
    int h = movables[0] -> spriteHeight;
    SDL_Rect camera = findCamera(x, y, w, h);
    SDL_Rect rectTo;

    // Render things
    for (unsigned int i = 0; i < movables.size(); i++) {
        rectTo.x = movables[i] -> x;
        rectTo.y = movables[i] -> y;
        rectTo.w = movables[i] -> spriteWidth;
        rectTo.h = movables[i] -> spriteHeight;

        // Convert the rectangle to screen coordinates
        rectTo = convertRect(rectTo, camera);

        assert(i != 0 || rectTo.x >= 0);
        assert(i != 0 || rectTo.y >= 0);
        assert(i != 0 || rectTo.x < camera.w);
        assert(i != 0 || rectTo.y < camera.h);

        // Draw!
        // TODO: check whether it's actually anywhere near the screen
        SDL_RenderCopy(renderer, movables[i] -> texture, NULL, &rectTo);
    }

}

// Update the screen
void WindowHandler::update(const Map &map, const vector<Movable *> &movables) {
    // Clear the screen
    SDL_RenderClear(renderer);

    // Put a black rectangle in the background
    SDL_Rect fillRect = { 0, 0, screenWidth, screenHeight };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x99, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);

    // Only draw stuff if it isn't minimized
    if (!isMinimized) {

        // Put stuff on it
        // movables[0] should be the player
        int x = movables[0] -> x;
        int y = movables[0] -> y;
        int w = movables[0] -> spriteWidth;
        int h = movables[0] -> spriteHeight;
        SDL_Rect camera = findCamera(x, y, w, h);
        renderMap(map, camera);

        // Draw any movables
        renderMovables(movables);

        // Update the screen
        SDL_RenderPresent(renderer);
    }
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
