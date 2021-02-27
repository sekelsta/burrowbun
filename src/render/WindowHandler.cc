#include <cassert>
#include <iostream>
#include <vector>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "WindowHandler.hh"

// Include things that were forward declared
#include "../Light.hh"
#include "../world/Tile.hh"
#include "../world/Map.hh"
#include "../entity/Player.hh"
#include "Sprite.hh"
#include "../Action.hh"
#include "../Rect.hh"
#include "../world/World.hh"

using namespace std;

// Return a rectangle in world coordinates for a player at x, y
// w and h are the width and height of the player sprite
Rect WindowHandler::findCamera(int x, int y, int w, int h) {
    Rect camera;
    camera.x = x - screenWidth / 2;
    camera.y = y - screenHeight / 2;
    camera.w = screenWidth;
    camera.h = screenHeight;
    camera.worldWidth = worldWidth;

    // Make the camera center be the center of the player
    camera.x += w / 2;
    camera.y += h / 2;

    // Adjust the camera if necessary, so that it's entirely on the screen
    // Because modulo can return a negative
    camera.x += worldWidth;
    camera.x %= worldWidth;
    camera.y = min(camera.y, worldHeight - screenHeight);
    camera.y = max(0, camera.y);

    // If, God forbid, the map is smaller than the camera, shrink the camera
    camera.w = min(camera.w, worldWidth - TILE_HEIGHT);
    camera.h = min(camera.h, worldHeight - TILE_WIDTH);

    return camera;
}

void WindowHandler::renderUI(Player &player, string path) {
    // Render the hotbar
    player.hotbar.render(path);

    // Render the stat bars
    player.healthBar.y = screenHeight - player.healthBar.distFromBottom;
    player.fullnessBar.y = screenHeight - player.fullnessBar.distFromBottom;
    player.manaBar.y = screenHeight - player.manaBar.distFromBottom;

    // And actually draw them
    player.healthBar.render();
    player.fullnessBar.render();
    player.manaBar.render();

    // Render the inventory, if necessary
    if (player.isInventoryOpen) {
        player.inventory.render(path);
        player.trash.render(path);
    }

    /* TODO: Note to self: when adding chests, make sure to not have infinite
    inventory textures for each one that has ever been opened. */

    // Render the item held by the mouse, if any
    if (player.mouseSlot != NULL) {
        // Find the mouse
        int x;
        int y;
        SDL_GetMouseState(&x, &y);
        // Make a rect to render to
        SDL_Rect rect;
        rect.w = ACTION_SPRITE_SIZE;
        rect.h = ACTION_SPRITE_SIZE;
        // Put it on the center of the mouse
        rect.x = x - (rect.w / 2);
        rect.y = y - (rect.h / 2);
        // Render
        player.mouseSlot -> render(rect, path);
    }
}

// Constructor
WindowHandler::WindowHandler(int screenWidth, int screenHeight, 
        int tileWidth, int tileHeight) 
    : screenWidth(screenWidth), screenHeight(screenHeight), 
        TILE_WIDTH(tileWidth), TILE_HEIGHT(tileHeight) {
    window = NULL;
    screenSurface = NULL;

    // Set the 2D vector of rects for the tiles
    resize(screenWidth, screenHeight);

    init();
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
        int newSize = ceil((float)screenHeight / (float)TILE_HEIGHT) + 1;
        tileRects[i].resize(newSize);
        // Set the tileRects to have the correct width and height
        for (unsigned j = 0; j < tileRects[i].size(); j++) {
            tileRects[i][j].w = TILE_WIDTH;
            tileRects[i][j].h = TILE_HEIGHT;
        }
    }
}

void WindowHandler::setMapSize(int tilesWide, int tilesHigh) {
    worldWidth = tilesWide * TILE_WIDTH;
    worldHeight = tilesHigh * TILE_HEIGHT;
}

// Start up the window
void WindowHandler::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        /* Yarr if I ever see a char[] again I will eat it. */
        string message = (string)"SDL could not initialize. SDL_Error: " 
                + SDL_GetError() + "\n";
        throw message;
    }
    else {
        // Create window
        window = SDL_CreateWindow("Burrowbun", SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight,
                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (window == NULL) {
            string message = (string)"Window could not be created. SDL_Error: " 
                    + SDL_GetError() + "\n";
            throw message;
        }
        else {
            // Create a renderer for the window
            Renderer::m.lock();
            Renderer::renderer = SDL_CreateRenderer(window, -1, 
                                            SDL_RENDERER_ACCELERATED);

            // Fall back to a software renderer if necessary
            if (Renderer::renderer == NULL) {
                cerr << "Hardware-accelerated renderer could not be created. ";
                cerr << "SDL Error: " << endl << SDL_GetError() << endl;
                cerr << "Falling back to software renderer." << endl;
                Renderer::renderer = SDL_CreateRenderer(window, -1, 
                                            SDL_RENDERER_SOFTWARE);
            }
            if (Renderer::renderer == NULL) {
                Renderer::m.unlock();
                string message = (string)"Software-accelerated renderer could "
                        + "not be created. SDL_Error: " + SDL_GetError() + "\n";
                throw message;
            }
            else {
                Renderer::m.unlock();
                // Initialize renderer draw color
                Renderer::setColorWhite();

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    string message = (string)"SDL_image could not initialize. "
                            + "SDL_image Error: " + IMG_GetError() + "\n";
                    throw message;
                }

                // Initialize font rendering
                if (TTF_Init() < 0) {
                    string message = (string)"SDL_ttf could not initialize. "
                            + "SDL_ttf error: " + TTF_GetError() + "\n";
                    throw message;
                }
            }
        }
    }
    // Assume the window is not minimized
    isMinimized = false;
}

// Render everything the map holds information about
// x and y are the center of view of the camera, in pixels, 
// where y = 0 at the bottom
// If either value puts the camera past the end of the map, it will be fixed
void WindowHandler::renderMap(Map &m, const Rect &camera) {
    // Make sure the renerer draw color is set to white
    Renderer::setColorWhite();

    assert(camera.x >= 0);
    assert(camera.y >= 0);

    // Rectangle to draw to
    SDL_Rect rectTo;
    rectTo.w = TILE_WIDTH;
    rectTo.h = TILE_HEIGHT;

    // Iterate through every tile at least partially within the camera
    int width = ceil((float)camera.w / (float)TILE_WIDTH) + 1;
    int height = ceil((float)camera.h / (float)TILE_HEIGHT) + 1;
    int xMapStart = ((camera.x / TILE_WIDTH) + m.getWidth()) % m.getWidth();
    int yMapStart = (camera.y + camera.h) / TILE_HEIGHT;

    /* Make sure the lights are updated. */
    m.setLight(xMapStart, yMapStart - height, xMapStart + width, yMapStart);

    assert(width != 0);
    assert(height != 0);
    for (int i = 0; i < width; i++) {
        rectTo.x = i * TILE_WIDTH - (camera.x % TILE_WIDTH);
        for (int j = 0; j < height; j++) {
            // Remember that screen y == 0 at the top but world y == 0 at 
            // the bottom. Here j == 0 at the top of the screen.
            // We're not using convertRect because that doesn't align them
            // with the tile grid.
            rectTo.y = (camera.h + camera.y) % TILE_HEIGHT;
            rectTo.y += (j - 1) * TILE_HEIGHT;

            // Render the tile
            int xTile = (xMapStart + i) % m.getWidth();
            int yTile = yMapStart - j;
            // But only if it's a tile that exists on the map
            assert (0 <= xTile);
            assert (xTile < m.getWidth());
            if (!m.isOnMap(xTile, yTile)) {
                continue;
            }

            /* Modulate the color due to lighting. */
            Light light = m.getLight(xTile, yTile);
            light.a = 255;
            uint8_t background = m.getBackgroundSprite(xTile, yTile);
            uint8_t foreground = m.getForegroundSprite(xTile, yTile);
            m.getBackground(xTile, yTile) -> render(background, light, rectTo);
            m.getForeground(xTile, yTile) -> render(foreground, light, rectTo);
        }
    }
}

// Update the screen
void WindowHandler::update(World &world) {
    /* Find the camera. */
    int w = world.player.getWidth();
    int h = world.player.getHeight();
    Rect camera = findCamera(world.player.getRect().x, 
        world.player.getRect().y, w, h);
    /* Tell the player where on the screen they are. This is only used by
    EventHandler. TODO: remove. */
    SDL_Rect playerRect = { world.player.getRect().x, 
        world.player.getRect().y, w, h };
    world.player.convertRect(playerRect, camera);
    world.player.screenX = playerRect.x;
    world.player.screenY = playerRect.y + playerRect.h;


    // Make sure the renderer isn't rendering to a texture
    Renderer::setTarget(NULL);
    // Clear the screen
    Renderer::renderClear();;

    // Put a black rectangle in the background
    SDL_Rect fillRect = { 0, 0, screenWidth, screenHeight };
    Light skyColor = world.map.getSkyColor();
    SDL_SetRenderDrawColor(Renderer::renderer, skyColor.r, skyColor.g, 
        skyColor.b, 0xFF);
    SDL_RenderFillRect(Renderer::renderer, &fillRect);

    // Only draw stuff if it isn't minimized
    if (!isMinimized) {
        renderMap(world.map, camera);

        // Draw any movables
        for (unsigned int i = 0; i < world.entities.size(); i++) {
            world.entities[i] -> render(camera);
        }
        for (unsigned int i = 0; i < world.droppedItems.size(); i++) {
            world.droppedItems[i] -> render(camera);
        }

        // Draw the UI
        renderUI(world.player, world.map.path);

        // Update the screen
        SDL_RenderPresent(Renderer::renderer);
    }
}

// Close the window, clean up, and exit SDL
void WindowHandler::close() {
    /* Destroy window and renderer. */
    Renderer::m.lock();
    SDL_DestroyRenderer(Renderer::renderer);
    Renderer::renderer = NULL;
    Renderer::m.unlock();
    SDL_DestroyWindow(window);
    window = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
