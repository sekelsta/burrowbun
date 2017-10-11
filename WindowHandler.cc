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
#include "Rect.hh"

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

// Render the texture to a 2d grid with width columns and height rows
void WindowHandler::renderGrid(Sprite &sprite, int width, int height) {
    // Where to render to
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = sprite.getWidth();
    rectTo.h = sprite.getHeight();
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            sprite.render(rectTo);
            rectTo.y += rectTo.h;
        }
        rectTo.x += rectTo.w;
        rectTo.y = 0;
    }
}

// Render each texture from textures onto to, using the spacing variables
// from hotbar. The texture to is expected to have the correct width and
// height, and the vector is expected to have length 12. 
SDL_Texture *WindowHandler::renderHotbarPart(const Hotbar &hotbar,
        vector<Sprite> textures, SDL_Texture *texture) const {
    assert(textures.size() == 12);
    // Make a texture if necessary
    if (texture == NULL) {
        // Create texture to draw to
        // Get the width and height of the textures to render
        // This function assumes they are all the same
        int width = hotbar.frame.getWidth();
        int height = hotbar.frame.getHeight();
        int totalWidth = 12 * width + 12 * hotbar.smallGap;
        totalWidth += 2 * hotbar.largeGap;
        Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888;
        texture = SDL_CreateTexture(Renderer::renderer, pixelFormat, 
        SDL_TEXTUREACCESS_TARGET, totalWidth, height);
        // Make the new texture have a transparent background
        SDL_SetRenderTarget(Renderer::renderer, texture);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(Renderer::renderer, 0, 0, 0 ,0);
        SDL_RenderClear(Renderer::renderer);
    }

    // Set render settings
    SDL_SetRenderTarget(Renderer::renderer, texture);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Set the draw color to white so it draws whatever it's drawing normally
    Renderer::setColorWhite();

    // Actually render
    SDL_Rect rectTo;
    rectTo.w = hotbar.frame.getWidth();
    rectTo.h = hotbar.frame.getHeight();
    SDL_Rect refRect = rectTo;
    // For each slot
    for (int i = 0; i < 12; i++) {
        // We know the clickboxes have the correct spacing, but the first one 
        // probably isn't at 0, 0. So we just correct for that.
        refRect.x = hotbar.clickBoxes[i].x - hotbar.clickBoxes[0].x;
        refRect.y = hotbar.clickBoxes[i].y - hotbar.clickBoxes[0].y;
        // Put rectTo in the middle of refRect
        rectTo.w = textures[i].getWidth();
        rectTo.h = textures[i].getHeight();
        rectTo.x = refRect.x + (refRect.w - rectTo.w) / 2;
        rectTo.y = refRect.y + (refRect.h - rectTo.h) / 2;
        textures[i].render(rectTo);
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
    int width = 12 * hotbar.frame.getWidth() + 12 * hotbar.smallGap;
    width += 2 * hotbar.largeGap + hotbar.offsetRight;
    int height = hotbar.frame.getHeight() + hotbar.offsetDown;
    hotbar.sprite.rect.w = width;
    hotbar.sprite.rect.h = height;
    Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888;
    // Actually create the texture
    Texture *all;
    if (hotbar.sprite.texture == NULL) {
        all = new Texture(pixelFormat, SDL_TEXTUREACCESS_TARGET, width, height);
        // Set the sprite thing in the hotbar to the texture we just made
        hotbar.sprite.texture.reset(all);
    }
    else {
        all = hotbar.sprite.texture.get();
    }


    // Fill a vector with frame images to render
    // The frame to put around each sprite
    vector<Sprite> frontFrames;
    vector<Sprite> backFrames;
    // The image of the item in the slot
    vector<Sprite> frontSprites;
    vector<Sprite> backSprites;
    for (int i = 0; i < 12; i++) {
        frontFrames.push_back(hotbar.frame);
        backFrames.push_back(hotbar.frame);
        // Use the other version if that key is selected
        if (hotbar.selected == i) {
            frontFrames[i].rect.y += hotbar.frame.getHeight();
        }
        else if (hotbar.selected == i + 12) {
            backFrames[i].rect.y += hotbar.frame.getHeight();
        }

        // Add the Action sprites to their lists of textures, if there is one 
        // in that slot.
        if (hotbar.actions[i] != NULL) {
            // Add a sprite to the front row
            // Load the sprite if it doesn't have one
            frontSprites.push_back(hotbar.actions[i] -> sprite);
        }
        else {
            frontSprites.push_back(Sprite());
        }
        if (hotbar.actions[i + 12] != NULL) {
            // Load the sprite if necessary
            backSprites.push_back(hotbar.actions[i + 12] -> sprite);
        }
        else {
            backSprites.push_back(Sprite());
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

    all -> SetRenderTarget();
    // Tell SDL to do transparency when it renders
    all -> SetTextureBlendMode(SDL_BLENDMODE_BLEND);
    // Set draw color to transparent so the texture has a transparent 
    // background
    SDL_SetRenderDrawColor(Renderer::renderer, 0, 0, 0 ,0);
    SDL_RenderClear(Renderer::renderer);

    // Actually render the sprite onto the texture
    Renderer::setColorWhite();
    // TODO: make the back layer slightly transparent
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = width - hotbar.offsetRight;
    rectTo.h = height - hotbar.offsetDown;
    // Render the back layer
    SDL_RenderCopy(Renderer::renderer, back, NULL, &rectTo);
    // Render the front layer
    rectTo.x = hotbar.offsetRight;
    rectTo.y = hotbar.offsetDown;
    SDL_RenderCopy(Renderer::renderer, front, NULL, &rectTo);

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
    SDL_SetRenderTarget(Renderer::renderer, NULL);

    // The rectangle to draw to
    SDL_Rect rectTo;
    rectTo.w = inventory.sprite.getWidth();
    assert(rectTo.w == inventory.squareSprite.getWidth() 
            * inventory.getWidth());
    rectTo.h = inventory.sprite.getHeight();
    assert(rectTo.h == inventory.squareSprite.getHeight() 
            * inventory.getHeight());
    rectTo.x = inventory.x;
    rectTo.y = inventory.y;

    // And actually render
    inventory.sprite.render(rectTo);
}

/* Draw the whole inventory onto a single sprite. */
void WindowHandler::updateInventorySprite(Inventory &inventory) {
    // Here seems like as good a place as any to tell the inventory to figure
    // out where it cares about being clicked
    inventory.updateClickBoxes();
    // Create a texture to draw it on
    Uint32 pixelFormat = inventory.squareSprite.texture -> getFormat();
    int width = inventory.squareSprite.getWidth() * inventory.getWidth();
    int height = inventory.squareSprite.getHeight() * inventory.getHeight();
    Texture *texture = new Texture(pixelFormat, 
            SDL_TEXTUREACCESS_TARGET, width, height);
    inventory.sprite.texture.reset(texture);

    // Store the size of the texture
    inventory.sprite.rect.w = width;
    inventory.sprite.rect.h = height;
    inventory.sprite.rect.x = 0;
    inventory.sprite.rect.y = 0;

    // Tell the renderer to draw to the texture
    texture -> SetRenderTarget();

    // Now loop through each square twice, once to draw the background and
    // once to draw the frame.
    // Draw the background
    renderGrid(inventory.squareSprite, inventory.getWidth(), 
            inventory.getHeight());

    // Render the items
    // Create a rectangle to draw them to
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = inventory.squareSprite.getWidth();
    rectTo.h = inventory.squareSprite.getHeight();
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
                // Center rectTo inside refRect
                rectTo.w = item -> sprite.getWidth();
                rectTo.h = item -> sprite.getHeight();
                rectTo.x = refRect.x + (refRect.w - rectTo.w) / 2;
                rectTo.y = refRect.y + (refRect.h - rectTo.h) / 2;
                item -> sprite.render(rectTo);
            }
            refRect.x += refRect.w;
        }
        refRect.x = 0;
        refRect.y += refRect.h;
    }

    // Now render the frames
    renderGrid(inventory.frameSprite, inventory.getWidth(), 
            inventory.getHeight());

    // And now the sprite is updated
    inventory.isSpriteUpdated = true;

    /* Make sure the render target is set to render to the window again. */
    SDL_SetRenderTarget(Renderer::renderer, NULL);
}

void WindowHandler::renderUI(Player &player) {
    // Re-render the hotbar sprite if necessary
    if (!player.hotbar.isSpriteUpdated) {
        updateHotbarSprite(player.hotbar);
    }

    // Make sure the renderer isn't rendering to a texture
    SDL_SetRenderTarget(Renderer::renderer, NULL);

    // Create a rect to render to
    SDL_Rect rectTo;
    rectTo.w = player.hotbar.sprite.getWidth();
    rectTo.h = player.hotbar.sprite.getHeight();

    // Render
    rectTo.x = player.hotbar.xStart;
    rectTo.y = player.hotbar.yStart;
    player.hotbar.sprite.render(rectTo);

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
        renderInventory(player.inventory);
        renderInventory(player.trash);
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
        rect.w = player.mouseSlot -> sprite.getWidth();
        rect.h = player.mouseSlot -> sprite.getHeight();
        // Put it on the center of the mouse
        rect.x = x - (rect.w / 2);
        rect.y = y - (rect.h / 2);
        // Render
        player.mouseSlot -> sprite.render(rect);
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
        window = SDL_CreateWindow("Hello whirreled!", SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight,
                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (window == NULL) {
            string message = (string)"Window could not be created. SDL_Error: " 
                    + SDL_GetError() + "\n";
            throw message;
        }
        else {
            // Create a renderer for the window
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
                string message = (string)"Software-accelerated renderer could "
                        + "not be created. SDL_Error: " + SDL_GetError() + "\n";
                throw message;
            }
            else {
                // Initialize renderer draw color
                Renderer::setColorWhite();

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    string message = (string)"SDL_image could not initialize. "
                            + "SDL_image Error: " + IMG_GetError() + "\n";
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
                break;
            }

            /* Modulate the color due to lighting. */
            Light light = m.getLight(xTile, yTile);
            uint8_t background = m.getBackgroundSprite(xTile, yTile);
            uint8_t foreground = m.getForegroundSprite(xTile, yTile);
            m.getBackground(xTile, yTile) -> render(background, light, rectTo);
            m.getForeground(xTile, yTile) -> render(foreground, light, rectTo);
        }
    }
}

// Update the screen
void WindowHandler::update(Map &map, 
        const vector<movable::Movable *> &movables, Player &player) {
    /* Find the camera. */
    int w = player.getWidth();
    int h = player.getHeight();
    Rect camera = findCamera(player.x, player.y, w, h);
    /* Tell the player where on the screen they are. This is only used by
    EventHandler. TODO: remove. */
    SDL_Rect playerRect = { player.x, player.y, w, h };
    player.convertRect(playerRect, camera);
    player.screenX = playerRect.x;
    player.screenY = playerRect.y + playerRect.h;


    // Make sure the renderer isn't rendering to a texture
    SDL_SetRenderTarget(Renderer::renderer, NULL);
    // Clear the screen
    SDL_RenderClear(Renderer::renderer);

    // Put a black rectangle in the background
    SDL_Rect fillRect = { 0, 0, screenWidth, screenHeight };
    SDL_SetRenderDrawColor(Renderer::renderer, 0x00, 0x99, 0xFF, 0xFF);
    SDL_RenderFillRect(Renderer::renderer, &fillRect);

    // Only draw stuff if it isn't minimized
    if (!isMinimized) {
        renderMap(map, camera);

        // Draw any movables
        for (unsigned int i = 0; i < movables.size(); i++) {
            movables[i] -> render(camera);
        }

        // Draw the UI
        renderUI(player);

        // Update the screen
        SDL_RenderPresent(Renderer::renderer);
    }
}

// Close the window, clean up, and exit SDL
void WindowHandler::close() {
    /* Destroy window and renderer. */
    SDL_DestroyRenderer(Renderer::renderer);
    Renderer::renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}
