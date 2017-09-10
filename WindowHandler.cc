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
    camera.y = min(camera.y, worldHeight - screenHeight);
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

// Set the renderer draw color to a Light color
void WindowHandler::setRenderColorToLight(const Light &color) {
    SDL_SetRenderDrawColor(Renderer::renderer, color.r, color.g, color.b, 0xFF);
}

// Render the texture to a 2d grid with width columns and height rows
void WindowHandler::renderGrid(const Sprite &sprite, int width, int height) {
    // Where to render to
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = sprite.rect.w;
    rectTo.h = sprite.rect.h;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            sprite.render(&rectTo);
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
    SDL_RenderFillRect(Renderer::renderer, &rect);

    // Draw the part that can regenerate ("part")
    rect.x += bar.full;
    rect.w = bar.part - bar.full;
    assert(rect.w >= 0);
    setRenderColorToLight(bar.partColor);
    SDL_RenderFillRect(Renderer::renderer, &rect);

    // Draw the empty part of the bar
    rect.x += rect.w;
    rect.w = bar.totalWidth - bar.part;
    setRenderColorToLight(bar.emptyColor);
    SDL_RenderFillRect(Renderer::renderer, &rect);

    // And draw the overlay on top
    // The magic numbers come from the width of the stat bar border
    int borderWidth = 1;
    rect.x = bar.x - borderWidth;
    rect.w = bar.totalWidth + 2 * borderWidth;
    rect.y -= borderWidth;
    rect.h += 2 * borderWidth;
    statBarOverlay.texture -> render(NULL, &rect); 
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
        int width = hotbar.frame.rect.w;
        int height = hotbar.frame.rect.h;
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
    SDL_SetRenderDrawColor(Renderer::renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Actually render
    SDL_Rect rectTo;
    rectTo.w = hotbar.frame.rect.w;
    rectTo.h = hotbar.frame.rect.h;
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
        textures[i].render(&rectTo);
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
    int width = 12 * hotbar.frame.rect.w + 12 * hotbar.smallGap;
    width += 2 * hotbar.largeGap + hotbar.offsetRight;
    int height = hotbar.frame.rect.w + hotbar.offsetDown;
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
            frontFrames[i].rect.y += hotbar.frame.rect.h;
        }
        else if (hotbar.selected == i + 12) {
            backFrames[i].rect.y += hotbar.frame.rect.h;
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
    SDL_SetRenderDrawColor(Renderer::renderer, 0xFF, 0xFF, 0xFF, 0xFF);
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
    rectTo.w = inventory.sprite.rect.w;
    rectTo.h = inventory.sprite.rect.h;
    rectTo.x = inventory.x;
    rectTo.y = inventory.y;

    // And actually render
    inventory.sprite.render(&rectTo);
}

/* Draw the whole inventory onto a single sprite. */
void WindowHandler::updateInventorySprite(Inventory &inventory) {
    // Here seems like as good a place as any to tell the inventory to figure
    // out where it cares about being clicked
    inventory.updateClickBoxes();
    // Create a texture to draw it on
    Uint32 pixelFormat = Inventory::squareSprite.texture -> getFormat();
    int width = Inventory::squareSprite.rect.w * inventory.getWidth();
    int height = Inventory::squareSprite.rect.h * inventory.getHeight();
    Texture *texture = new Texture(pixelFormat, 
            SDL_TEXTUREACCESS_TARGET, width, height);

    // Store the size of the texture
    inventory.sprite.rect.w = width;
    inventory.sprite.rect.h = height;

    // Tell the renderer to draw to the texture
    texture -> SetRenderTarget();
    texture -> SetTextureBlendMode(SDL_BLENDMODE_BLEND);

    // Now loop through each square twice, once to draw the background and
    // once to draw the frame.
    // Draw the background
    Inventory::squareSprite.texture->SetTextureColorMod(inventory.squareColor);
    // Set the rect in the correct part of the spritesheet
    Inventory::squareSprite.rect.x = 0;
    Inventory::squareSprite.rect.y = 0;

    // And actually render
    // Render the background
    renderGrid(Inventory::squareSprite, inventory.getWidth(), 
            inventory.getHeight());

    // Set render draw color to white
    Inventory::squareSprite.texture -> SetTextureColorMod({0xFF, 0xFF, 0xFF, 
            0x00});

    // Render the items
    // Create a rectangle to draw them to
    SDL_Rect rectTo;
    rectTo.x = 0;
    rectTo.y = 0;
    rectTo.w = Inventory::squareSprite.rect.w;
    rectTo.h = Inventory::squareSprite.rect.h;
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
                rectTo.w = item -> sprite.rect.w;
                rectTo.h = item -> sprite.rect.h;
                rectTo.x = refRect.x + (refRect.w - rectTo.w) / 2;
                rectTo.y = refRect.y + (refRect.h - rectTo.h) / 2;
                item -> sprite.render(&rectTo);
            }
            refRect.x += refRect.w;
        }
        refRect.x = 0;
        refRect.y += refRect.h;
    }

    // Now render the frames
    // Make a spriteRect for the frame
    Inventory::squareSprite.rect.y = 0;
    Inventory::squareSprite.rect.x = Inventory::squareSprite.rect.w;
    Sprite frame = Inventory::squareSprite;

    // Actually render
    renderGrid(frame, inventory.getWidth(), inventory.getHeight());

    // Done rendering stuff. Give the inventory it's new sprite.
    inventory.sprite.texture.reset(texture);
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
    SDL_SetRenderTarget(Renderer::renderer, NULL);

    // Create a rect to render to
    SDL_Rect rectTo;
    rectTo.w = player.hotbar.sprite.rect.w;
    rectTo.h = player.hotbar.sprite.rect.h;

    // Render
    rectTo.x = player.hotbar.xStart;
    rectTo.y = player.hotbar.yStart;
    player.hotbar.sprite.render(&rectTo);

    // Render the stat bars
    player.healthBar.y = screenHeight - player.healthBar.distFromBottom;
    player.fullnessBar.y = screenHeight - player.fullnessBar.distFromBottom;
    player.manaBar.y = screenHeight - player.manaBar.distFromBottom;

    // And actually draw them
    renderStatBar(player.healthBar);
    renderStatBar(player.fullnessBar);
    renderStatBar(player.manaBar);

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
        rect.w = player.mouseSlot -> sprite.rect.w;
        rect.h = player.mouseSlot -> sprite.rect.h;
        // Put it on the center of the mouse
        rect.x = x - (rect.w / 2);
        rect.y = y - (rect.h / 2);
        // Render
        player.mouseSlot -> sprite.render(&rect);
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

    /* Info for what to render over the statbar rectangles. TODO: move
    elsewhere. */
    statBarOverlay.name = "stat_bar_overlay.png";
    statBarOverlay.loadTexture(UI_SPRITE_PATH);
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
                SDL_SetRenderDrawColor(Renderer::renderer, 0xFF, 0xFF, 0xFF, 0xFF);

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
void WindowHandler::renderMap(Map &m, const SDL_Rect &camera) {
    // Make sure the renerer draw color is set to white
    SDL_SetRenderDrawColor(Renderer::renderer, 0xFF, 0xFF, 0xFF, 0xFF);

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
    /* Location to hold which sprite on the sheet to render. */
    Location spritePlace; 
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

            Sprite backSprite = m.getBackground(xTile, yTile) -> sprite;
            Sprite foreSprite = m.getForeground(xTile, yTile) -> sprite;
            if (backSprite.hasTexture() || foreSprite.hasTexture()) {
                // Modulate the color due to lighting
                Light light = m.getLight(xTile, yTile);
                if (backSprite.hasTexture()) {
                    /* Use darkness. */
                    backSprite.texture -> SetTextureColorMod(light); 
                    uint8_t background = m.getBackgroundSprite(xTile, yTile);
                    SpaceInfo::fromSpritePlace(spritePlace, background);
                    backSprite.rect.x = spritePlace.x * TILE_WIDTH;
                    backSprite.rect.y = spritePlace.y * TILE_HEIGHT;
                    backSprite.render(&rectTo);
                }
                if (foreSprite.hasTexture()) {
                    foreSprite.texture -> SetTextureColorMod(light); 
                    uint8_t foreground = m.getForegroundSprite(xTile, yTile);
                    SpaceInfo::fromSpritePlace(spritePlace, foreground);
                    foreSprite.rect.x = spritePlace.x * TILE_WIDTH;
                    foreSprite.rect.y = spritePlace.y * TILE_HEIGHT;
                    foreSprite.render(&rectTo);
                }
            }
        }
    }
}

// Render any movables (the player, monsters NPCs, dropped items)
void WindowHandler::renderMovables(const vector<movable::Movable *> &movables) {
    // Make sure the renerer draw color is set to white
    SDL_SetRenderDrawColor(Renderer::renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Find where the player and camera are
    int x = movables[0] -> x;
    int y = movables[0] -> y;
    int w = movables[0] -> sprite.rect.w;
    int h = movables[0] -> sprite.rect.h;
    SDL_Rect camera = findCamera(x, y, w, h);
    SDL_Rect rectTo;

    // Render things
    for (unsigned int i = 0; i < movables.size(); i++) {
        rectTo.x = movables[i] -> x;
        rectTo.y = movables[i] -> y;
        rectTo.w = movables[i] -> sprite.rect.w;
        rectTo.h = movables[i] -> sprite.rect.h;
        // Convert the rectangle to screen coordinates
        rectTo = convertRect(rectTo, camera);

        assert(i != 0 || rectTo.x >= 0);
        assert(i != 0 || rectTo.y >= 0);
        assert(i != 0 || rectTo.x < camera.w);
        assert(i != 0 || rectTo.y < camera.h);

        // Draw!
        // TODO: check whether it's actually anywhere near the screen
        movables[i] -> sprite.render(&rectTo);
    }

}

// Update the screen
void WindowHandler::update(Map &map, 
        const vector<movable::Movable *> &movables, Player &player) {
    // Tell the player where on the screen it was drawn
    int w = player.sprite.rect.w;
    int h = player.sprite.rect.h;
    SDL_Rect camera = findCamera(player.x, player.y, w, h);
    SDL_Rect playerRect = { player.x, player.y, w, h };
    playerRect = convertRect(playerRect, camera);
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

        // Put stuff on it
        // movables[0] should be the player
        int x = movables[0] -> x;
        int y = movables[0] -> y;
        int w = movables[0] -> sprite.rect.w;
        int h = movables[0] -> sprite.rect.h;
        SDL_Rect camera = findCamera(x, y, w, h);
        renderMap(map, camera);

        // Draw any movables
        renderMovables(movables);

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
