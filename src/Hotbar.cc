#include <iostream>
#include "Hotbar.hh"
#include "filepaths.hh"

// The number of slots in the hotbar

using namespace std;

// Render each texture from textures onto to, using the spacing variables
// from hotbar. The texture to is expected to have the correct width and
// height, and the vector is expected to have length 12. 
SDL_Texture *Hotbar::renderHotbarPart(vector<Sprite> textures,
        SDL_Texture *texture) const {
    assert(textures.size() == 12);
    // Make a texture if necessary
    if (texture == NULL) {
        // Create texture to draw to
        // Get the width and height of the textures to render
        // This function assumes they are all the same
        int width = frame.getWidth();
        int height = frame.getHeight();
        int totalWidth = 12 * width + 12 * smallGap;
        totalWidth += 2 * largeGap;
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
    rectTo.w = frame.getWidth();
    rectTo.h = frame.getHeight();
    SDL_Rect refRect = rectTo;
    // For each slot
    for (int i = 0; i < 12; i++) {
        // We know the clickboxes have the correct spacing, but the first one 
        // probably isn't at 0, 0. So we just correct for that.
        refRect.x = clickBoxes[i].x - clickBoxes[0].x;
        refRect.y = clickBoxes[i].y - clickBoxes[0].y;
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
void Hotbar::updateSprite() {
    /* Sprite will soon be updated. */
    isSpriteUpdated = true;
    // Make the texture to render the sprite of the hotbar to
    // Hardcoding 12 because I don't expect to change my mind (12 is the
    // number of F keys).
    int width = 12 * frame.getWidth() + 12 * smallGap;
    width += 2 * largeGap + offsetRight;
    int height = frame.getHeight() + offsetDown;
    sprite.rect.w = width;
    sprite.rect.h = height;
    Uint32 pixelFormat = SDL_PIXELFORMAT_ARGB8888;
    // Actually create the texture
    Texture *all;
    if (sprite.texture == NULL) {
        all = new Texture(pixelFormat, SDL_TEXTUREACCESS_TARGET, width, height);
        // Set the sprite thing in the hotbar to the texture we just made
        sprite.texture.reset(all);
    }
    else {
        all = sprite.texture.get();
    }


    // Fill a vector with frame images to render
    // The frame to put around each sprite
    vector<Sprite> frontFrames;
    vector<Sprite> backFrames;
    // The image of the item in the slot
    vector<Sprite> frontSprites;
    vector<Sprite> backSprites;
    for (int i = 0; i < 12; i++) {
        frontFrames.push_back(frame);
        backFrames.push_back(frame);
        // Use the other version if that key is selected
        if (selected == i) {
            frontFrames[i].rect.y += frame.getHeight();
        }
        else if (selected == i + 12) {
            backFrames[i].rect.y += frame.getHeight();
        }

        // Add the Action sprites to their lists of textures, if there is one 
        // in that slot.
        if (actions[i].action != NULL) {
            // Add a sprite to the front row
            // Load the sprite if it doesn't have one
            frontSprites.push_back(actions[i].action -> sprite);
        }
        else {
            frontSprites.push_back(Sprite());
        }
        if (actions[i + 12].action != NULL) {
            // Load the sprite if necessary
            backSprites.push_back(actions[i + 12].action -> sprite);
        }
        else {
            backSprites.push_back(Sprite());
        }
    }
    SDL_Texture *front = renderHotbarPart(frontSprites, NULL);
    front = renderHotbarPart(frontFrames, front);
    SDL_Texture *back = renderHotbarPart(backSprites, NULL);
    back = renderHotbarPart(backFrames, back);

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
    rectTo.w = width - offsetRight;
    rectTo.h = height - offsetDown;
    // Render the back layer
    SDL_RenderCopy(Renderer::renderer, back, NULL, &rectTo);
    // Render the front layer
    rectTo.x = offsetRight;
    rectTo.y = offsetDown;
    SDL_RenderCopy(Renderer::renderer, front, NULL, &rectTo);

    // Not leak memory
    SDL_DestroyTexture(front);
    SDL_DestroyTexture(back);
}

// Constructor, which fills it with default values
Hotbar::Hotbar(string path) {
    // If you want to change these default settings, this is the place in the 
    // code to do it.
    smallGap = 4;
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

    clickBoxes.resize(24);

    int x = xStart + offsetRight;
    // For each section of four
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            int index = 4 * i + j;
            clickBoxes[index].x = x;
            clickBoxes[index].y = yStart + offsetDown;
            clickBoxes[index].w = frame.getWidth();
            clickBoxes[index].h = frame.getHeight();
            clickBoxes[index].wasClicked = false;
            clickBoxes[index].containsMouse = false;
            clickBoxes[index + 12].x = x - offsetRight;
            clickBoxes[index + 12].y = yStart;
            clickBoxes[index + 12].w = frame.getWidth();
            clickBoxes[index + 12].h = frame.getHeight();
            clickBoxes[index + 12].wasClicked = false;
            clickBoxes[index + 12].containsMouse = false;
            x += frame.getWidth() + smallGap;
        }
        x += largeGap;
    }

    // Set every action * to NULL
    actions.resize(24);
    for (unsigned int i = 0; i < actions.size(); i++) {
        actions[i] = {NULL, false};
    }

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
bool Hotbar::update(Action *mouse) {
    bool answer = false;
    for (unsigned int i = 0; i < clickBoxes.size(); i++) {
        // Ignore mouse button up or mouse button held down
        if (clickBoxes[i].wasClicked && !clickBoxes[i].isHeld
                && clickBoxes[i].event.type == SDL_MOUSEBUTTONDOWN ) {
            /* Select it if it wasn't already. */
            select(i);
            // See if we should put something in the slot
            if (clickBoxes[i].event.button == SDL_BUTTON_LEFT
                    && mouse != NULL) {
                actions[i] = {mouse, mouse -> isItem};
                // If it's an item, put it back in the inventory
                if (mouse -> isItem) {
                    answer = true;
                }
                /* And now the sprite needs to change. */
                isSpriteUpdated = false;
            }
            // If it was a right click, we should remove that item from the
            // hotbar.
            else if (clickBoxes[i].event.button == SDL_BUTTON_RIGHT) {
                actions[i] = {NULL, false};
                /* And again the sprite needs to change. */
                isSpriteUpdated = false;
            }
            /* Now we've used this click. */
            clickBoxes[i].wasClicked = false;
        }
    }
    return answer;
}

// Return the selected action
Action *Hotbar::getSelected() {
    return actions[selected].action;
}

void Hotbar::render() {
    // Re-render the sprite if necessary
    if (!isSpriteUpdated) {
        updateSprite();
    }

    // Make sure the renderer isn't rendering to a texture
    SDL_SetRenderTarget(Renderer::renderer, NULL);

    // Create a rect to render to
    SDL_Rect rectTo = {xStart, yStart, sprite.getWidth(), sprite.getHeight()};

    // Render
    sprite.render(rectTo);
}

