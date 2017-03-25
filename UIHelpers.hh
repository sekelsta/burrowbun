#ifndef UIHELPERS_HH
#define UIHELPERS_HH

#include <SDL2/SDL.h>
#include "Light.hh"

/* A struct with a rectangle and bools. The rectangle is in screen
coordinates, and the bools are for whether the mouse is in the box and 
whether it's been clicked. */
struct MouseBox {
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
};

/* A struct for drawing a bar, such as a health bar. The bool is for whether 
it's vertical or horizontal, although you can usually also tell that from 
whether it's taller or longer. The rectangles are for where on the screen to 
draw the bar, and the other ints are for the actual numbers represented.
Actually maybe I'll add a bool later, if I need to, but for now all
statbars are horizontal.  */
struct StatBar {
    // Basically three rectangles
    // Location of the top-left corner.
    int x;
    int y;
    // Assume it's horizontal and the height of the rectangles never changes
    int h;
    // Width of the part that has the stat.
    int full;
    // Width of the part of the bar that can regenerate without help
    int part;
    // Width of the entire bar.
    int totalWidth;

    // The actual stats
    int max;
    int fullStat;
    int partStat;

    // What color to draw the different rects
    Light fullColor;
    Light partColor;
    Light emptyColor;

private:
    // Translate from portion of max health to portion of bar filled
    int convert(int newValue) {
        if (max != 0) {
            float fraction = newValue / (float)max;
            return fraction * totalWidth;
        }
        else {
            return 0;
        }
    }

public:
 
    void setFull(int newValue) {
        fullStat = newValue;
        full = convert(newValue);
    }

    void setPart(int newValue) {
        partStat  = newValue;
        part = convert(newValue);
    }

    // Set the stat to as high as it can go
    void fill() {
        setPart(max);
        setFull(max);
    }
};


#endif
