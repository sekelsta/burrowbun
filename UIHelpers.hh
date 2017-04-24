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
    int maxStat;
    int fullStat;
    int partStat;

    // What color to draw the different rects
    Light fullColor;
    Light partColor;
    Light emptyColor;

private:
    // Translate from portion of max health to portion of bar filled
    int convert(int newValue) {
        if (maxStat != 0) {
            float fraction = newValue / (float)maxStat;
            return fraction * totalWidth;
        }
        else {
            return 0;
        }
    }

public:
    // Constructor
    StatBar() {
        fullStat = 0;
        partStat = 0;
    }

    // Set the amount of the stat
    void setFull(int newValue) {
        // Can't set it below 0 or above the max
        newValue = max(0, newValue);
        newValue = min(maxStat, newValue);
        fullStat = newValue;
        // And you can't recover a stat past the temporary cap
        if (fullStat > partStat) {
            fullStat = partStat;
        }
        full = convert(fullStat);
    }

    // Set the temporary cap (which prevents the stat from regenerating 
    // completely)
    void setPart(int newValue) {
        newValue  = max(0, newValue);
        newValue = min(maxStat, newValue);
        partStat = newValue;
        part = convert(newValue);
        if (fullStat > partStat) {
            setFull(partStat);
        }
    }

    // Add amount to the full part
    void addFull(int amount) {
        setFull(fullStat + amount);
    }

    // Add amount to the part part
    void addPart(int amount) {
        setPart(partStat + amount);
    }

    // Set the stat to as high as it can go
    void fill() {
        setPart(maxStat);
        setFull(maxStat);
    }
};


#endif
