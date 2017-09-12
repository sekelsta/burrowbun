#ifndef ANIMATION_HH
#define ANIMATION_HH

#include "Sprite.hh"
#include <vector>
#include "json.hpp"

/* A class to hold the information for an animation. */
class Animation {
    /* What frames to draw. */
    std::vector<Sprite> frames;
    /* Which frame to draw next. */
    int frame;

    /* Render itself. */
    inline void render(SDL_Rect &rect) {
        frames[frame].render(&rect);
    }

    /* No arguments constructor. */
    inline Animation() {
        Sprite sprite;
        sprite.name = "unloaded";
        frames.push_back(sprite);
        frame = 0;
        /* Attempting to render this animation will cause an assertionerror
        because it has no sprite loaded but the name is not "". */
    }

    inline Animation(const nlohmann::json &j) {
        frame = 0;
        frames = j.get<std::vector<Sprite>>();
    }
};

#endif
