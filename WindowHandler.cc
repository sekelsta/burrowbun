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
    camera.y = max(TILE_HEIGHT, camera.y);

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

// Render each texture from textures onto to, using the spacing variables
// from hotbar. The texture to is expected to have the correct width and
// height, and the vector is expected to have length 12. 
SDL_Texture *WindowHandler::renderHotbarPart(const Hotbar &hotbar,
        vector<SDL_Texture*> textures) const {
    assert(textures.size() == 12);
    // Create texture to draw to
    Uint32 pixelFormat;
    int width;
    int height;
    // Get the width and height of the textures to render
    // This function assumes they are all the same
    SDL_QueryTexture(textures.back(), &pixelFormat, NULL, &width, &height);
    int totalWidth = 12 * width + 12 * hotbar.smallGap + 2 * hotbar.largeGap;
    SDL_Texture *to = SDL_CreateTexture(renderer, pixelFormat, 
        SDL_TEXTUREACCESS_TARGET, totalWidth, height);

    // Set render settings
    SDL_SetRenderTarget(renderer, to);
    SDL_SetTextureBlendMode(to, SDL_BLENDMODE_BLEND);
    // Set draw color to transparent so the texture has a transparent 
    // background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0 ,0);
    SDL_RenderClear(renderer);

    // Set the draw color to white so it draws whatever it's drawing normally
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Actually render
    SDL_Rect rectTo;
    rectTo.w = width;
    rectTo.h = height;
    rectTo.x = 0;
    rectTo.y = 0;
    // For each slot
    for (int i = 0; i < 12; i++) {
        // We know the clickboxes have the correct spacing, but the first one 
        // probably isn't at 0, 0. So we just correct for that.
        rectTo.x = hotbar.clickBoxes[i].x - hotbar.clickBoxes[0].x;
        rectTo.y = hotbar.clickBoxes[i].y - hotbar.clickBoxes[0].y;
        SDL_RenderCopy(renderer, textures[i], NULL, &rectTo);
    }

    return to;
}

// Draw the entire hotbar sprite to a texture. This only needs to be called 
// when the hotbar is first made, or when anything about it changes.
void WindowHandler::updateHotbarSprite(Hotbar &hotbar) {
    // Make a texture and render the hotbar to it.
    // Use the same pixel format as the frame did.
    Uint32 pixelFormat;
    SDL_QueryTexture(textures.back(), &pixelFormat, NULL, NULL, NULL);
    // Make the texture to render the sprite of the hotbar to
    // Hardcoding 12 because I don't expect to change my mind (12 is the
    // number of F keys).
    int width = 12 * hotbar.frame.width + 12 * hotbar.smallGap;
    width += 2 * hotbar.largeGap + hotbar.offsetRight;
    int height = hotbar.frame.height + hotbar.offsetDown;
    hotbar.sprite.width = width;
    hotbar.sprite.height = height;
    // Create a texture to render the entire hotbar to
    SDL_Texture *all = SDL_CreateTexture(renderer, pixelFormat,
        SDL_TEXTUREACCESS_TARGET, width, height);

    // Fill a vector with frame images to render
    vector<SDL_Texture*> frontSprites;
    vector<SDL_Texture*> backSprites;
    for (int i = 0; i < 12; i++) {
        frontSprites.push_back(hotbar.frame.texture);
        backSprites.push_back(hotbar.frame.texture);
        // Use the other version if that key is selected
        if (hotbar.selected == i) {
            frontSprites[i] = hotbar.frameSelected.texture;
        }
        else if (hotbar.selected == i + 12) {
            backSprites[i] = hotbar.frameSelected.texture;
        }
    }
    SDL_Texture *front = renderHotbarPart(hotbar, frontSprites);
    SDL_Texture *back = renderHotbarPart(hotbar, backSprites);

    if (hotbar.isSwitched) {
        SDL_Texture *temp = front;
        front = back;
        back = temp;
    }

    SDL_SetRenderTarget(renderer, all);
    // Tell SDL to do trnsparency when it renders
    SDL_SetTextureBlendMode(all, SDL_BLENDMODE_BLEND);
    // Set draw color to transparent so the texture has a transparent 
    // background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0 ,0);
    SDL_RenderClear(renderer);

    // Actually render the sprite onto the texture
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    // TODO: make the back layer slightly transparent
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = width - hotbar.offsetRight;
    rectTo.h = height - hotbar.offsetDown;
    // Render the back layer
    SDL_RenderCopy(renderer, back, NULL, &rectTo);
    // Render the front layer
    rectTo.x = hotbar.offsetRight;
    rectTo.y = hotbar.offsetDown;
    SDL_RenderCopy(renderer, front, NULL, &rectTo);

    // Set the sprite thing in the hotbar to the texture we just made
    hotbar.sprite.texture = all;

    // Not leak memory
    textures.push_back(all);
    SDL_DestroyTexture(front);
    SDL_DestroyTexture(back);


    // Now the sprite has been updated
    hotbar.isSpriteUpdated = true;
}

// Currently this just renders the hotbar
void WindowHandler::renderUI(Hotbar &hotbar) {
    // Re-render the hotbar sprite if necessary
    if (!hotbar.isSpriteUpdated) {
        updateHotbarSprite(hotbar);
    }

    // Make sure the renderer isn't rendering to a texture
    SDL_SetRenderTarget(renderer, NULL);

    // Create a rect to render to
    SDL_Rect rectTo;
    rectTo.w = hotbar.sprite.width;
    rectTo.h = hotbar.sprite.height;

    // Render
    rectTo.x = hotbar.xStart;
    rectTo.y = hotbar.yStart;
    SDL_RenderCopy(renderer, hotbar.sprite.texture, NULL, &rectTo);
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
    // Assume the window is not minimized
    isMinimized = false;

    return success;
}

// Load all the pictures, return true if successful
bool WindowHandler::loadMedia(vector<Tile *> &pointers, 
        vector<Movable *> &movables, Hotbar &hotbar) {
    bool success = true;

    // Load the textures for tiles
    success = loadTiles(pointers);

    // And for the movables
    success = success && loadMovables(movables);

    // And load the hotbar
    // movables[0] is the player and therefore has a hotbar
    success = success && loadHotbar(hotbar);

    return success;
}

// Load an image onto a surface and convert it to match the screen
bool WindowHandler::loadTexture(const string &name) {
    bool success = true;
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
            cerr << "Failed to create texture from " <<  name << endl;
            cerr << "SDL Error: " << SDL_GetError() << endl;
        }

        // Get rid of the surface
        SDL_FreeSurface(surface);
    }

    // Add it to the list of textures so it can be deallocated later
    textures.push_back(texture);
    assert(textures.size() != 0);
    if (textures.back() == NULL) {
        success = false;
    }


    return success;
}

// Load a texture for each tile
bool WindowHandler::loadTiles(vector<Tile *> &pointers) {
    bool success = true;
    for (unsigned i = 0; i < pointers.size(); i++) {
        string name = TILE_PATH + pointers[i] -> sprite.name;
        if (name != TILE_PATH) {
            success = success && loadTexture(name);

            // Set the tile's texture to the loaded texture
            assert(pointers[i] -> sprite.texture == NULL);
            pointers[i] -> sprite.texture = textures.back();
        }
    }

    return success;
}

// Load a texture for each movable
bool WindowHandler::loadMovables(vector<Movable *> &movables) {
    bool success = true;
    for (unsigned int i = 0; i < movables.size(); i++) {
        string name = MOVABLE_PATH + movables[i] -> getSprite();
        success = success && loadTexture(name);

        movables[i] -> texture = textures.back();
    }

    return success;
}

// Load textures for the hotbar
bool WindowHandler::loadHotbar(Hotbar &hotbar) {
    bool success = true;

    string name = UI_PATH + hotbar.frame.name;
    success = loadTexture(name);
    hotbar.frame.texture = textures.back();

    // Load the picture of a selected frame
    name = UI_PATH + hotbar.frameSelected.name;
    success = success && loadTexture(name);
    hotbar.frameSelected.texture = textures.back();

    // Update the hotbar sprite
    updateHotbarSprite(hotbar);

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
            assert (0 <= xTile);
            assert (xTile < mapWidth);
            assert (0 <= yTile); 
            assert (yTile < worldHeight / TILE_HEIGHT);
            SDL_Texture *texture 
                = m.getForeground(xTile, yTile) -> sprite.texture;
            if (texture != NULL) {
                // Modulate the color due to lighting
                Light light;
                light.sum(m.getSkyLight(xTile, yTile), 
                    m.getBlockLight(xTile, yTile));
                // Only add darkness if darkness is enabled
                if(enableDarkness) {
                    SDL_SetTextureColorMod(texture, light.r, light.g, light.b);
                }
                rectFrom.x = m.getSpritePlace(xTile, yTile).x * TILE_WIDTH;
                rectFrom.y = m.getSpritePlace(xTile, yTile).y * TILE_HEIGHT;
                SDL_RenderCopy(renderer, texture, &rectFrom, rectTo);
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
void WindowHandler::update(const Map &map, const vector<Movable *> &movables, 
        Hotbar &hotbar) {
    // Make sure the renderer isn't rendering to a texture
    SDL_SetRenderTarget(renderer, NULL);
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

        // Draw the UI
        renderUI(hotbar);

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
