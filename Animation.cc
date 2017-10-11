#include "Animation.hh"

void Animation::render(const SDL_Rect &rectTo) {
        assert(frame < (int)frames.size() * delay);
        /* Make sure the width and height are correct. */
        assert(rectTo.w == frames[frame / delay].w);
        assert(rectTo.h == frames[frame / delay].h);

        SDL_Rect rectFrom;
        rectFrom.w = rectTo.w;
        rectFrom.h = rectTo.h;
        rectFrom.x = frames[frame / delay].x;
        rectFrom.y = frames[frame / delay].y;
        SpriteBase::render(rectFrom, rectTo);
        frame++;
        frame %= frames.size() * delay;
}

int Animation::getWidth() const {
    return frames[frame / delay].w;
}

int Animation::getHeight() const {
    return frames[frame / delay].h;
}


