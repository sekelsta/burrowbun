#ifndef ANIMATION_HH
#define ANIMATION_HH

#include "Sprite.hh"
#include <vector>
#include <string>
#include "json.hpp"

/* A class to hold the information for an animation. */
class Animation {
    /* What frames to draw. */
    std::vector<Sprite> frames;
    /* Which frame to draw next. */
    int frame;

    /* How long to stay on each frame. */
    int delay;

public:
    /* Render itself. */
    inline void render(SDL_Rect &rect) {
        assert(frame < (int)frames.size() * delay);
        /* Make sure the width and height are correct. */
        assert(rect.w == frames[frame/ delay].rect.w);
        assert(rect.h == frames[frame / delay].rect.h);
        frames[frame / delay].render(&rect);
        frame++;
        frame %= frames.size() * delay;
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

    inline Animation(const nlohmann::json &j, std::string prefix) {
        frame = 0;
        delay = j["delay"];
        frames = j["frames"].get<std::vector<Sprite>>();
        for (unsigned int i = 0; i < frames.size(); i++) {
            frames[i].loadTexture(prefix);
        }
    }

    /* Return an SDL_Rect with the width and height of the next frame. */
    inline SDL_Rect getRect() const {
        assert(frame < (int)frames.size() * delay);
        return frames[frame / delay].rect;
    }
};

#endif
