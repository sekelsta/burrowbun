#include <iostream>
#include "Hotbar.hh"
#include "filepaths.hh"
#include "Texture.hh"

// The number of slots in the hotbar
#define KEYLABEL_FONT_SIZE 12
using namespace std;

// Render each texture from textures onto to, using the spacing variables
// from hotbar. The texture to is expected to have the correct width and
// height, and the vector is expected to have length 12. 
Texture *Hotbar::renderHotbarPart(int row, string path, 
        Texture *texture, int left, int up) const {
    assert(row == 0 || row == 1);

    // Set render settings
    texture -> SetRenderTarget();
    texture -> SetTextureBlendMode(SDL_BLENDMODE_BLEND);

    // Set the draw color to white so it draws whatever it's drawing normally
    Renderer::setColorWhite();

    // Actually render
    SDL_Rect refRect;
    refRect.w = frame.getWidth();
    refRect.h = frame.getHeight();
    // For each slot
    for (int i = 0; i < 12; i++) {
        // We know the clickboxes have the correct spacing, but the first one 
        // probably isn't at 0, 0. So we just correct for that.
        refRect.x = clickBoxes[row][i].x - clickBoxes[1][0].x + left;
        refRect.y = clickBoxes[row][i].y - clickBoxes[1][0].y + up;
        /* Render the item. */
        // TODO
        /*
        if (actions[row][i]) {
            actions[row][i] -> render(refRect, path);
        }
        */
        if (false) {}
        else if (items[row][i]) {
            items[row][i] -> render(refRect, path);
        }

        /* Render the frame. */
        Sprite f = frame;
        if (selected == 12 * row + i) {
            f.rect.y += frame.getHeight();
        }
        f.render(refRect);
    }

    return texture;
}

// Draw the entire hotbar sprite to a texture. This only needs to be called 
// when the hotbar is first made, or when anything about it changes.
void Hotbar::updateSprite(string path) {
    /* Sprite will soon be updated. */
    isSpriteUpdated = true;

    /* Labels, plus extra distance up to move the letters. */
    int up = Texture::getTextHeight(KEYLABEL_FONT_SIZE, 
        "F1", DEFAULT_OUTLINE_SIZE) / 2;
    vector<vector<string>> labels = {
        {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "="},
        {"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", 
        "F11", "F12"}};
    // Make the texture to render the sprite of the hotbar to
    // Hardcoding 12 because I don't expect to change my mind (12 is the
    // number of F keys).
    int width = 12 * frame.getWidth() + 12 * smallGap;
    width += 2 * largeGap + offsetRight + up;
    int height = frame.getHeight() + offsetDown + up;
    sprite.rect.w = width;
    sprite.rect.h = height;
    Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888;
    // Actually create the texture
    Texture *all;
    if (sprite.texture == nullptr) {
        all = new Texture(pixelFormat, SDL_TEXTUREACCESS_TARGET, width, height);
        // Set the sprite thing in the hotbar to the texture we just made
        sprite.texture.reset(all);
    }
    else {
        all = sprite.texture.get();
    }

    all -> SetRenderTarget();
    // Tell SDL to do transparency when it renders
    all -> SetTextureBlendMode(SDL_BLENDMODE_BLEND);
    // Set draw color to transparent so the texture has a transparent 
    // background
    SDL_SetRenderDrawColor(Renderer::renderer, 0, 0, 0 ,0);
    SDL_RenderClear(Renderer::renderer);

    /* Render */
    renderHotbarPart(1, path, all, up, up);
    renderHotbarPart(0, path, all, up, up);

    /* Render the key / number labels. */
    for (unsigned int i = 0; i < clickBoxes.size(); i++) {
        for (unsigned int j = 0; j < clickBoxes[i].size(); j++) {
            /* Render the label. */
            Texture text(labels[i][j], path, KEYLABEL_FONT_SIZE, 0);
            int x = clickBoxes[i][j].x - clickBoxes[1][0].x;
            int y = clickBoxes[i][j].y - clickBoxes[1][0].y;
            text.render(x, y);
        }
    }
}

// Constructor, which fills it with default values
Hotbar::Hotbar(string path) : Inventory(12, 2, path) {
    // If you want to change these default settings, this is the place in the 
    // code to do it.
    smallGap = 8;
    largeGap = 16;
    offsetRight = 0;
    offsetDown = 36;
    // Where to draw the hotbar
    xStart = 20;
    yStart = 10;

    /* TODO: move magic numbers to a json file or something. */
    /* x, y, w, h, name */
    frame = Sprite(0, 0, 32, 32, "frame.png");

    int spriteWidth = 12 * frame.getWidth() + 12 * smallGap + 2 * largeGap 
        + offsetRight;
    int spriteHeight = frame.getHeight() + offsetDown;
    sprite = Sprite(0, 0, spriteWidth, spriteHeight, "");

    isSpriteUpdated = false;

    selected = 0;

    int x = xStart + offsetRight;
    // For each section of four
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            int index = 4 * i + j;
            clickBoxes[0][index].x = x;
            clickBoxes[0][index].y = yStart + offsetDown;
            clickBoxes[0][index].w = frame.getWidth();
            clickBoxes[0][index].h = frame.getHeight();
            clickBoxes[0][index].wasClicked = false;
            clickBoxes[0][index].containsMouse = false;
            clickBoxes[1][index].x = x - offsetRight;
            clickBoxes[1][index].y = yStart;
            clickBoxes[1][index].w = frame.getWidth();
            clickBoxes[1][index].h = frame.getHeight();
            clickBoxes[1][index].wasClicked = false;
            clickBoxes[1][index].containsMouse = false;
            x += frame.getWidth() + smallGap;
        }
        x += largeGap;
    }

    // Set every action * to NULL
    // TODO
    /*
    actions.resize(24);
    for (unsigned int i = 0; i < actions.size(); i++) {
        actions[i] = {NULL, false};
    }
    */

    /* Load the frame's texture. */
    frame.loadTexture(path + UI_SPRITE_PATH);
}

// Select a slot
void Hotbar::select(int slot) {
    if (slot != selected) {
        selected = slot;
        isSpriteUpdated = false;
    }
}

// Use mouse input, return true if the item the mouse was holding should
// be put in the inventory
void Hotbar::update(Action *&mouse, bool isInvOpen) {
    Inventory::update();
    if (isInvOpen && (!mouse || mouse -> isItem())) {
        Inventory::update_internal(mouse);
    }
    for (unsigned int row = 0; row < clickBoxes.size(); row++) {
        for (unsigned int col = 0; col < clickBoxes[row].size(); col++) {
            // Ignore mouse button up or mouse button held down
            if (clickBoxes[row][col].wasClicked && !clickBoxes[row][col].isHeld
                    && clickBoxes[row][col].event.type == SDL_MOUSEBUTTONDOWN ) {
                /* Select it if it wasn't already. */
                select(row * clickBoxes[row].size() + col);
                // See if we should put something in the slot
                if (clickBoxes[row][col].event.button == SDL_BUTTON_LEFT
                        && mouse) {
                    // TODO
                    /* And now the sprite needs to change. */
                    isSpriteUpdated = false;
                }
                // If it was a right click, we should remove that item from the
                // hotbar.
                else if (clickBoxes[row][col].event.button == SDL_BUTTON_RIGHT) {
                    // TODO
                    //actions[i].action == nullptr;
                    /* And again the sprite needs to change. */
                    isSpriteUpdated = false;
                }
                /* Now we've used this click. */
                clickBoxes[row][col].wasClicked = false;
            }
        }
    }
    Inventory::resetClicks();
}

// Return the selected action
Action *Hotbar::getSelected() {
    // TODO
    /*
    return actions[selected].action;
    */
    return items[selected / 12][selected % 12];
}

void Hotbar::render(string path) {
    // Re-render the sprite if necessary
    if (!isSpriteUpdated) {
        updateSprite(path);
    }

    // Make sure the renderer isn't rendering to a texture
    SDL_SetRenderTarget(Renderer::renderer, NULL);

    // Create a rect to render to
    SDL_Rect rectTo = {xStart, yStart, sprite.getWidth(), sprite.getHeight()};

    // Render
    sprite.render(rectTo);
}

