#ifndef BUTTON_HH
#define BUTTON_HH

#include "Sprite.hh"
#include "Light.hh"
#include "Rect.hh"
#include <string>

/* A class with a rectangle and bools. The rectangle is in screen
coordinates, and the bools are for whether the mouse is in the box and 
whether it's been clicked. */
class MouseBox {
    friend class EventHandler;
protected:
    // x and y values of the top left corner of the rectangle
    int x;
    int y;
    // Width and height of the rectangle
    int w;
    int h;
    // Self-explanatory
    bool containsMouse;
    // Whether we should pay any attention to the event this contains
    bool wasClicked;
    // Whether it was also clicked last time (presumably the mouse button is 
    // being held down)
    bool isHeld;
    // The event that happened in this box and needs to be handled
    SDL_MouseButtonEvent event;

    // Return true if the coordinates are inside the rectangle, and also
    // sets containsMouse to the return value
    bool contains(int xMouse, int yMouse) {
        bool answer = (x <= xMouse) && (xMouse < x + w);
        answer = answer && (y <= yMouse) && (yMouse < y + h);
        containsMouse = answer;
        return answer;
    }

public:
    /* Access functions for getting information about the mouse. */
    inline bool hasMouse() {
        return containsMouse;
    }

    inline bool clicked() {
        return wasClicked && !isHeld && event.type == SDL_MOUSEBUTTONDOWN;
    } 

    inline bool leftClicked() {
        return clicked() && event.button == SDL_BUTTON_LEFT;
    }

    inline bool rightClicked() {
        return clicked() && event.button == SDL_BUTTON_RIGHT;
    }

    /* Access functions for getting information about location. */
    inline int getX() {
        return x;
    }

    inline int getY() {
        return y;
    }

    /* Move to a new location. */
    inline void move(Rect rect) {
        x = rect.x;
        y = rect.y;
        w = rect.w;
        h = rect.h;
    }

    inline void move(int newx, int newy, int neww, int newh) {
        x = newx;
        y = newy;
        w = neww;
        h = newh;
    }

    /* Reset input to none. */
    inline void reset() {
        wasClicked = false;
        containsMouse = false;
    }
};

struct Button : public MouseBox {
    Sprite sprite;
    Light mouse;
    Light noMouse;

    inline void render() {
        if (hasMouse()) {
            sprite.setColorMod(mouse);
        }
        else {
            sprite.setColorMod(noMouse);
        }
        sprite.render({x, y, w, h});
    }
};

#endif
