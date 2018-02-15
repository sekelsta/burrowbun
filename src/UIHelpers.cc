#include "UIHelpers.hh"
#include "filepaths.hh"
#include "Renderer.hh"
#include <SDL2/SDL.h>

void StatBar::render() {
    SDL_Rect rect;
    rect.y = y;
    rect.h = h;

    // Draw the part that's full
    rect.x = x;
    rect.w = full;
    Renderer::setColor(fullColor);
    SDL_RenderFillRect(Renderer::renderer, &rect);

    // Draw the part that can regenerate ("part")
    rect.x += full; 
    rect.w = part - full;
    assert(rect.w >= 0);
    Renderer::setColor(partColor);
    SDL_RenderFillRect(Renderer::renderer, &rect);

    // Draw the empty part of the bar
    rect.x += rect.w;
    rect.w = totalWidth - part;
    Renderer::setColor(emptyColor);
    SDL_RenderFillRect(Renderer::renderer, &rect);

    // And draw the overlay on top
    // The magic number comes from the width of the stat bar border
    int borderWidth = 1;
    rect.x = x - borderWidth;
    rect.w = totalWidth + 2 * borderWidth;
    rect.y -= borderWidth;
    rect.h += 2 * borderWidth;
    overlay.render(rect);

    /* Leave the renderer on white. */
    Renderer::setColorWhite();
}

/* Get a statbar from a json. */
void from_json(const nlohmann::json &j, StatBar &bar) {
    bar.totalWidth = j["totalWidth"];
    bar.h = j["h"];
    bar.x = j["x"];
    bar.y = 0;
    bar.distFromBottom = j["distFromBottom"];

    bar.fullColor = j["fullColor"].get<Light>();
    bar.partColor = j["partColor"].get<Light>();
    bar.emptyColor = j["emptyColor"].get<Light>();

    bar.overlay = j["overlay"].get<Sprite>();
}


