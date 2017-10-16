#ifndef ANIMATION_HH
#define ANIMATION_HH

#include "SpriteBase.hh"
#include "Rect.hh"
#include <vector>
#include <string>
#include "json.hpp"

/* A class to hold the information for an animation. */
class Animation: public SpriteBase {
    /* What frames to draw. */
    std::vector<SDL_Rect> frames;
    /* Which frame to draw next. */
    int frame;

    /* How long to stay on each frame. */
    int delay;

public:
    /* Render itself. */
    virtual void render(const SDL_Rect &rect);

    /* No arguments constructor. */
    inline Animation() {
        name = "unloaded";
        frame = 0;
        /* Attempting to render this animation will cause an assertionerror
        because it has no sprite loaded but the name is not "". */
    }

    inline Animation(const nlohmann::json &j, std::string prefix) {
        frame = 0;
        delay = j["delay"];
        name = j["name"];
        std::vector<Rect> frameRects = j["frames"].get<std::vector<Rect>>();
        for (unsigned int i = 0; i < frameRects.size(); i++) {
            SDL_Rect sdlrect;
            sdlrect.x = frameRects[i].x;
            sdlrect.y = frameRects[i].y;
            sdlrect.w = frameRects[i].w;
            sdlrect.h = frameRects[i].h;
            frames.push_back(sdlrect);
        }
        loadTexture(prefix);
    }

    virtual int getWidth() const;
    virtual int getHeight() const;
    /* Returns the rect for the current frame (the one that will be renderered
    next). */
    virtual Rect getRect() const;

    /* Return an SDL_Rect with the width and height of the next frame. */
    inline SDL_Rect getSDLRect() const {
        assert(frame < (int)frames.size() * delay);
        return frames[frame / delay];
    }
};

#endif
