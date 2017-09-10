#include "UIHelpers.hh"
#include "filepaths.hh"

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

    /* And now's as good a time as any to load the sprite. */
    bar.overlay.loadTexture(UI_SPRITE_PATH);
}



