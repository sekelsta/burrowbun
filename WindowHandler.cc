#include <cassert>
#include <iostream>
#include <vector>
#include <SDL2/SDL_image.h>
#include "WindowHandler.hh"

// Include things that were forward declared
#include "Light.hh"
#include "Tile.hh"
#include "Map.hh"
#include "Hotbar.hh"
#include "Player.hh"
#include "UIHelpers.hh"
#include "Sprite.hh"
#include "Inventory.hh"
#include "Action.hh"

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

// Set the renderer draw color to a Light color
void WindowHandler::setRenderColorToLight(const Light &color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0xFF);
}

// Render the texture to a 2d grid with width columns and height rows
void WindowHandler::renderGrid(const SpriteRect &sprite, int width, int height) {
    // Where to render to
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = sprite.rect.w;
    rectTo.h = sprite.rect.h;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            sprite.render(renderer, &rectTo);
            rectTo.y += rectTo.h;
        }
        rectTo.x += rectTo.w;
        rectTo.y = 0;
    }
}

// Render a StatBar
void WindowHandler::renderStatBar(StatBar &bar) {
    SDL_Rect rect;
    rect.y = bar.y;
    rect.h = bar.h;

    // Draw the part that's full
    rect.x = bar.x;
    rect.w = bar.full;
    setRenderColorToLight(bar.fullColor);
    SDL_RenderFillRect(renderer, &rect);

    // Draw the part that can regenerate ("part")
    rect.x += bar.full;
    rect.w = bar.part - bar.full;
    assert(rect.w >= 0);
    setRenderColorToLight(bar.partColor);
    SDL_RenderFillRect(renderer, &rect);

    // Draw the empty part of the bar
    rect.x += rect.w;
    rect.w = bar.totalWidth - bar.part;
    setRenderColorToLight(bar.emptyColor);
    SDL_RenderFillRect(renderer, &rect);

    // And draw the overlay on top
    // The magic numbers come from the width of the stat bar border
    int borderWidth = 1;
    rect.x = bar.x - borderWidth;
    rect.w = bar.totalWidth + 2 * borderWidth;
    rect.y -= borderWidth;
    rect.h += 2 * borderWidth;
    SDL_RenderCopy(renderer, statBarOverlay.texture, NULL, &rect); 
}

// Render each texture from textures onto to, using the spacing variables
// from hotbar. The texture to is expected to have the correct width and
// height, and the vector is expected to have length 12. 
SDL_Texture *WindowHandler::renderHotbarPart(const Hotbar &hotbar,
        vector<SpriteRect> textures, SDL_Texture *texture) const {
    assert(textures.size() == 12);
    // Make a texture if necessary
    if (texture == NULL) {
        // Create texture to draw to
        // Get the width and height of the textures to render
        // This function assumes they are all the same
        int width = hotbar.frame.width;
        int height = hotbar.frame.height;
        int totalWidth = 12 * width + 12 * hotbar.smallGap;
        totalWidth += 2 * hotbar.largeGap;
        Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888;
        texture = SDL_CreateTexture(renderer, pixelFormat, 
        SDL_TEXTUREACCESS_TARGET, totalWidth, height);
        // Make the new texture have a transparent background
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0 ,0);
        SDL_RenderClear(renderer);
    }

    // Set render settings
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Set the draw color to white so it draws whatever it's drawing normally
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Actually render
    SDL_Rect rectTo;
    rectTo.w = hotbar.frame.width;
    rectTo.h = hotbar.frame.height;
    SDL_Rect refRect = rectTo;
    // For each slot
    for (int i = 0; i < 12; i++) {
        // We know the clickboxes have the correct spacing, but the first one 
        // probably isn't at 0, 0. So we just correct for that.
        refRect.x = hotbar.clickBoxes[i].x - hotbar.clickBoxes[0].x;
        refRect.y = hotbar.clickBoxes[i].y - hotbar.clickBoxes[0].y;
        // Put rectTo in the middle of refRect
        rectTo.w = textures[i].rect.w;
        rectTo.h = textures[i].rect.h;
        rectTo.x = refRect.x + (refRect.w - rectTo.w) / 2;
        rectTo.y = refRect.y + (refRect.h - rectTo.h) / 2;
        textures[i].render(renderer, &rectTo);
    }

    return texture;
}

// Draw the entire hotbar sprite to a texture. This only needs to be called 
// when the hotbar is first made, or when anything about it changes.
void WindowHandler::updateHotbarSprite(Hotbar &hotbar) {
    /* Sprite will soon be updated. */
    hotbar.isSpriteUpdated = true;
    // Make the texture to render the sprite of the hotbar to
    // Hardcoding 12 because I don't expect to change my mind (12 is the
    // number of F keys).
    int width = 12 * hotbar.frame.width + 12 * hotbar.smallGap;
    width += 2 * hotbar.largeGap + hotbar.offsetRight;
    int height = hotbar.frame.height + hotbar.offsetDown;
    hotbar.sprite.width = width;
    hotbar.sprite.height = height;
    Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888;
    // Actually create the texture
    SDL_Texture *all = NULL;
    if (hotbar.sprite.texture == NULL) {
        all = SDL_CreateTexture(renderer, pixelFormat,
                SDL_TEXTUREACCESS_TARGET, width, height);
        textures.push_back(all);
        // Set the sprite thing in the hotbar to the texture we just made
        hotbar.sprite.texture = all;
    }
    else {
        all = hotbar.sprite.texture;
    }


    // Fill a vector with frame images to render
    // The frame to put around each sprite
    vector<SpriteRect> frontFrames;
    vector<SpriteRect> backFrames;
    // The image of the item in the slot
    vector<SpriteRect> frontSprites;
    vector<SpriteRect> backSprites;
    for (int i = 0; i < 12; i++) {
        frontFrames.push_back(SpriteRect(hotbar.frame));
        backFrames.push_back(SpriteRect(hotbar.frame));
        // Use the other version if that key is selected
        if (hotbar.selected == i) {
            frontFrames[i].rect.y += hotbar.frame.height;
        }
        else if (hotbar.selected == i + 12) {
            backFrames[i].rect.y += hotbar.frame.height;
        }

        // Add the Action sprites to their lists of textures, if there is one 
        // in that slot.
        if (hotbar.actions[i] != NULL) {
            // Add a sprite to the front row
            // Load the sprite if it doesn't have one
            loadAction(*hotbar.actions[i]);
            frontSprites.push_back(SpriteRect(hotbar.actions[i] -> sprite));
        }
        else {
            frontSprites.push_back(SpriteRect());
        }
        if (hotbar.actions[i + 12] != NULL) {
            // Load the sprite if necessary
            loadAction(*hotbar.actions[i + 12]);
            backSprites.push_back(SpriteRect(hotbar.actions[i + 12] -> sprite));
        }
        else {
            backSprites.push_back(SpriteRect());
        }
    }
    SDL_Texture *front = renderHotbarPart(hotbar, frontSprites, NULL);
    front = renderHotbarPart(hotbar, frontFrames, front);
    SDL_Texture *back = renderHotbarPart(hotbar, backSprites, NULL);
    back = renderHotbarPart(hotbar, backFrames, back);

    if (hotbar.isSwitched) {
        SDL_Texture *temp = front;
        front = back;
        back = temp;
    }

    SDL_SetRenderTarget(renderer, all);
    // Tell SDL to do transparency when it renders
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

    // Not leak memory
    SDL_DestroyTexture(front);
    SDL_DestroyTexture(back);
}

// Render the inventory to the screen
void WindowHandler::renderInventory(Inventory &inventory) {
    if (!inventory.isSpriteUpdated) {
        updateInventorySprite(inventory);
    }
    // Just in case, make sure the renderer is rendering to the screen
    SDL_SetRenderTarget(renderer, NULL);

    // The rectangle to draw to
    SDL_Rect rectTo;
    rectTo.w = inventory.sprite.width;
    rectTo.h = inventory.sprite.height;
    rectTo.x = inventory.x;
    rectTo.y = inventory.y;

    // And actually render
    SDL_RenderCopy(renderer, inventory.sprite.texture, NULL, &rectTo);
}


/* Load the spritesheet used by all inventories. */
bool WindowHandler::loadInventory() {
    // Inventory::squareSprite is a static member
    // Hard-code sprite info? not sure if this is the best way.
    Inventory::squareSprite.texture = NULL;
    Inventory::squareSprite.name = "inventory.png";
    Inventory::squareSprite.width = 32;
    Inventory::squareSprite.height = 32;
    Inventory::squareSprite.rows = 1;
    Inventory::squareSprite.cols = 2;

    // And actually load it
    bool success = loadTexture(UI_PATH + Inventory::squareSprite.name);
    if (success) {
        Inventory::squareSprite.texture = textures.back();
    }
    return success;
}

/* Draw the whole inventory onto a single sprite. */
void WindowHandler::updateInventorySprite(Inventory &inventory) {
    // Here seems like as good a place as any to tell the inventory to figure
    // out where it cares about being clicked
    inventory.updateClickBoxes();
    // Unload the current inventory sprite, if there is one
    unloadTexture(inventory.sprite.texture);
    inventory.sprite.texture = NULL;
    // Create a texture to draw it on
    Uint32 pixelFormat;
    SDL_QueryTexture(Inventory::squareSprite.texture, &pixelFormat, 
        NULL, NULL, NULL);
    int width = Inventory::squareSprite.width * inventory.getWidth();
    int height = Inventory::squareSprite.height * inventory.getHeight();
    SDL_Texture *texture = SDL_CreateTexture(renderer, pixelFormat, 
        SDL_TEXTUREACCESS_TARGET, width, height);
    textures.push_back(texture);

    // Store the size of the texture
    inventory.sprite.width = width;
    inventory.sprite.height = height;

    // Tell the renderer to draw to the texture
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Now loop through each square twice, once to draw the background and
    // once to draw the frame.
    // Draw the background
    Light color = inventory.squareColor;
    SDL_SetTextureColorMod(Inventory::squareSprite.texture, color.r, color.g, 
        color.b);
    // Set the rect in the correct part of the spritesheet
    Inventory::squareSprite.row = 0;
    Inventory::squareSprite.col = 0;
    SpriteRect backgroundSquare = SpriteRect(Inventory::squareSprite);

    // And actually render
    // Render the background
    renderGrid(backgroundSquare, inventory.getWidth(), inventory.getHeight());

    // Set render draw color to white
    SDL_SetTextureColorMod(Inventory::squareSprite.texture, 0xFF, 0xFF, 0xFF);

    // Render the items
    // Create a rectangle to draw them to
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = Inventory::squareSprite.width;
    rectTo.h = Inventory::squareSprite.height;
    // Rectangle that sits in each square, to refer to when the item isn't
    // the same size as the square
    SDL_Rect refRect = rectTo;
    // Loop through and render each item
    for (int row = 0; row < inventory.getHeight(); row++) {
        for (int col = 0; col < inventory.getWidth(); col++) {
            // Create a spriterect from the item and render it, if the item
            // exists
            Item *item = inventory.getItem(row, col);
            if (item != NULL) {
                // Load the sprite if necessary
                // Remember, loadAction does nothing if it already has a sprite
                loadAction(*item);
                // Center rectTo inside refRect
                rectTo.w = item -> sprite.width;
                rectTo.h = item -> sprite.height;
                rectTo.x = refRect.x + (refRect.w - rectTo.w) / 2;
                rectTo.y = refRect.y + (refRect.h - rectTo.h) / 2;
                SpriteRect(item -> sprite).render(renderer, &rectTo);
            }
            refRect.x += refRect.w;
        }
        refRect.x = 0;
        refRect.y += refRect.h;
    }

    // Now render the frames
    // Make a spriteRect for the frame
    Inventory::squareSprite.row = 0;
    Inventory::squareSprite.col = 1;
    SpriteRect frame = SpriteRect(Inventory::squareSprite);

    // Actually render
    renderGrid(frame, inventory.getWidth(), inventory.getHeight());

    // Done rendering stuff. Give the inventory it's new sprite.
    inventory.sprite.texture = texture;
    // And now the sprite is updated
    inventory.isSpriteUpdated = true;
}

// Currently this just renders the hotbar
void WindowHandler::renderUI(Player &player) {
    // Re-render the hotbar sprite if necessary
    if (!player.hotbar.isSpriteUpdated) {
        updateHotbarSprite(player.hotbar);
    }

    // Make sure the renderer isn't rendering to a texture
    SDL_SetRenderTarget(renderer, NULL);

    // Create a rect to render to
    SDL_Rect rectTo;
    rectTo.w = player.hotbar.sprite.width;
    rectTo.h = player.hotbar.sprite.height;

    // Render
    rectTo.x = player.hotbar.xStart;
    rectTo.y = player.hotbar.yStart;
    SDL_RenderCopy(renderer, player.hotbar.sprite.texture, NULL, &rectTo);

    // Render the stat bars
    // Reference distance from the bottom of the screen
    int up = 90;
    // Reference distance from the left side of the screen
    int right = 30;

    int refX = right;
    int refY = screenHeight - up;

    // The magic numbers here come from the positions required by the sprite I
    // want to put the bars next to. The refX and refY are its coordinates.
    // These numbers are set here in case the window size changed.
    // TODO: move magic numbers to json file
    player.healthBar.x = refX + 65;
    player.healthBar.y = refY + 17;
    player.fullnessBar.x = refX + 65;
    player.fullnessBar.y = refY + 29;
    player.manaBar.x = refX + 65;
    player.manaBar.y = refY + 41;
    // And actually draw them
    renderStatBar(player.healthBar);
    renderStatBar(player.fullnessBar);
    renderStatBar(player.manaBar);

    // Render the inventory, if necessary
    if (player.isInventoryOpen) {
        renderInventory(player.inventory);
        renderInventory(player.trash);
    }
    // Otherwise make sure its sprite is cleaned up
    else if (player.inventory.sprite.texture != NULL) {
        unloadTexture(player.inventory.sprite.texture);
        player.inventory.sprite.texture = NULL;
        player.inventory.isSpriteUpdated = false;
        unloadTexture(player.trash.sprite.texture);
        player.trash.sprite.texture = NULL;
        player.trash.isSpriteUpdated = false;
    }

    // Render the item held by the mouse, if any
    if (player.mouseSlot != NULL) {
        // Load the action sprite if it issn't already
        loadAction(*player.mouseSlot);
        // Find the mouse
        int x;
        int y;
        SDL_GetMouseState(&x, &y);
        // Make a rect to render to
        SDL_Rect rect;
        rect.w = player.mouseSlot -> sprite.width;
        rect.h = player.mouseSlot -> sprite.height;
        // Put it on the center of the mouse
        rect.x = x - (rect.w / 2);
        rect.y = y - (rect.h / 2);
        // Render
        SpriteRect spriteRect(player.mouseSlot -> sprite);
        spriteRect.render(renderer, &rect);

        
    }
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

    // Not sure if this belongs here, but set the name of file to get the
    // texture for statBarOverlay from
    statBarOverlay.name = "stat_bar_overlay.png";
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
        vector<movable::Movable *> &movables, Hotbar &hotbar) {
    bool success = true;

    // Load the overlay for the player's stats
    success = loadTexture(UI_PATH + statBarOverlay.name);
    assert(textures.size() != 0);
    statBarOverlay.texture = textures.back(); 

    // Load the textures for tiles
    success = success && loadTiles(pointers);

    // And for the movables
    success = success && loadMovables(movables);

    // And load the hotbar
    // movables[0] is the player and therefore has a hotbar
    success = success && loadHotbar(hotbar);

    // Load inventory background sprite
    success = success && loadInventory();

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
        assert(false);
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

// Unload a texture. After calling this, make sure to set all pointers to the
// texture to NULL. Returns whether it succeeded at destroying the texture.
bool WindowHandler::unloadTexture(SDL_Texture *texture) {
    // Loop through textures until it is found
    bool success = false;
    for (unsigned int i = 0; i < textures.size(); i++) {
        if (textures[i] == texture) {
            // Free the texture if we havn't already and if it isn't null
            if (!success && !texture) {
                SDL_DestroyTexture(texture);
                success = true;
            }
            // And now we definately have destroyed the texture, so set this
            // pointer to null
            textures[i] = NULL;
        }
    }
    return success;
}

// Load a texture for each tile
bool WindowHandler::loadTiles(vector<Tile *> &pointers) {
    bool success = true;
    for (unsigned i = 0; i < pointers.size(); i++) {
        string name = TILE_PATH + pointers[i] -> sprite.name;
        /* Sprites that have no name shouldn't be rendered. */
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
bool WindowHandler::loadMovables(vector<movable::Movable *> &movables) {
    bool success = true;
    for (unsigned int i = 0; i < movables.size(); i++) {
        string name = MOVABLE_PATH + movables[i] -> sprite.name;
        success = success && loadTexture(name);

        movables[i] -> sprite.texture = textures.back();
    }

    return success;
}

// Load textures for the hotbar
bool WindowHandler::loadHotbar(Hotbar &hotbar) {
    bool success = true;

    string name = UI_PATH + hotbar.frame.name;
    success = loadTexture(name);
    hotbar.frame.texture = textures.back();

    // Update the hotbar sprite
    updateHotbarSprite(hotbar);

    return success;
}

// Load an item sprite
// Assumes the item has no sprite to start with
bool WindowHandler::loadAction(Action &action) {
    // Ignore if there's already a sprite loaded
    if (action.sprite.texture != NULL) {
        return false;
    }
    bool success = loadTexture(ICON_PATH + action.sprite.name);
    if (success) {
        action.sprite.texture = textures.back();
    }
    return success;
}

// Render everything the map holds information about
// x and y are the center of view of the camera, in pixels, 
// where y = 0 at the bottom
// If either value puts the camera past the end of the map, it will be fixed
void WindowHandler::renderMap(Map &m, const SDL_Rect &camera) {
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
                Light light = m.getLight(xTile, yTile);
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
void WindowHandler::renderMovables(const vector<movable::Movable *> &movables) {
    // Make sure the renerer draw color is set to white
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Find where the player and camera are
    int x = movables[0] -> x;
    int y = movables[0] -> y;
    int w = movables[0] -> sprite.width;
    int h = movables[0] -> sprite.height;
    SDL_Rect camera = findCamera(x, y, w, h);
    SDL_Rect rectTo;

    // Render things
    for (unsigned int i = 0; i < movables.size(); i++) {
        rectTo.x = movables[i] -> x;
        rectTo.y = movables[i] -> y;
        rectTo.w = movables[i] -> sprite.width;
        rectTo.h = movables[i] -> sprite.height;

        // Convert the rectangle to screen coordinates
        rectTo = convertRect(rectTo, camera);

        assert(i != 0 || rectTo.x >= 0);
        assert(i != 0 || rectTo.y >= 0);
        assert(i != 0 || rectTo.x < camera.w);
        assert(i != 0 || rectTo.y < camera.h);

        // Draw!
        // TODO: check whether it's actually anywhere near the screen
        SDL_RenderCopy(renderer, movables[i] -> sprite.texture, NULL, &rectTo);
    }

}

// Update the screen
void WindowHandler::update(Map &map, 
        const vector<movable::Movable *> &movables, Player &player) {
    // Tell the player where on the screen it was drawn
    int w = player.sprite.width;
    int h = player.sprite.height;
    SDL_Rect camera = findCamera(player.x, player.y, w, h);
    SDL_Rect playerRect = { player.x, player.y, w, h };
    playerRect = convertRect(playerRect, camera);
    player.screenX = playerRect.x;
    player.screenY = playerRect.y + playerRect.h;

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
        int w = movables[0] -> sprite.width;
        int h = movables[0] -> sprite.height;
        SDL_Rect camera = findCamera(x, y, w, h);
        renderMap(map, camera);

        // Draw any movables
        renderMovables(movables);

        // Draw the UI
        renderUI(player);

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
