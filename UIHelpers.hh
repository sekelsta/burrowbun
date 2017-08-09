#ifndef UIHELPERS_HH
#define UIHELPERS_HH

#include <iostream>
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
    // Location of the top-left corner. y will depend on window size and should
    // be set by WindowHandler
    int x;
    int y;
    /* Distance between the bottom of the screen and what the y value
    sould be. */
    int distFromBottom;
    // Assume it's horizontal and the height of the rectangles never changes
    int h;
    // Width of the part that has the stat.
    int full;
    // Width of the part of the bar that can regenerate without help
    int part;
    // Width of the entire bar.
    int totalWidth;

    // The actual stats
    double maxStat;
    double fullStat;
    double partStat;

    /* Affects how fast it regenerates. */
    double linearRegen;
    double percentRegen;
    double quadraticRegen;
    /* How much faster more time passing since being damaged makes it 
    regenerate. */
    double timeRegen;
    int ticksUntilRegen;
    int baseRegenTicks;

    // What color to draw the different rects
    Light fullColor;
    Light partColor;
    Light emptyColor;

private:
    // Translate from portion of max health to portion of bar filled
    int convert(double newValue) {
        if (maxStat != 0) {
            double fraction = newValue / maxStat;
            return fraction * totalWidth;
        }
        else {
            return 0;
        }
    }

    /* Slightly increase the amount of fullStat. */
    void regenerate() {
        double total = 0;
        if (ticksUntilRegen < 0) {
            total = percentRegen * (maxStat + partStat);
            total += quadraticRegen * fullStat;
            total *= -1 * timeRegen * ticksUntilRegen;
        }
        total += linearRegen;
        addFull(total);
    }

public:
    // Constructor
    StatBar() {
        fullStat = 0;
        partStat = 0;
        linearRegen = 0;
        percentRegen = 0;
        quadraticRegen = 0;
        timeRegen = 0;
        ticksUntilRegen = 0;
        baseRegenTicks = 1;
    }

    // Set the amount of the stat
    void setFull(double newValue) {
        // Can't set it below 0 or above the max
        newValue = max(0.0, newValue);
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
    void setPart(double newValue) {
        newValue  = max(0.0, newValue);
        newValue = min(maxStat, newValue);
        partStat = newValue;
        part = convert(newValue);
        if (fullStat > partStat) {
            setFull(partStat);
        }
    }

    // Add amount to the full part
    void addFull(double amount) {
        /* If we lost the stat, we shouldn't start regenerating right away. */
        if (amount < 0) {
            resetRegen();
        }
        setFull(fullStat + amount);
    }

    // Add amount to the part part
    void addPart(double amount) {
        /* Again, reset regen. */
        if (amount < 0) {
            resetRegen();
        }
        setPart(partStat + amount);
    }

    // Set the stat to as high as it can go
    void fill() {
        setPart(maxStat);
        setFull(maxStat);
    }

    /* Reset the time until it can regenerate again. */
    void resetRegen() {
        ticksUntilRegen = baseRegenTicks;
    }

    /* Do the things! */
    void update() {
        regenerate();
        ticksUntilRegen -= 1;
    }
};

/* Get a statbar from a json. */
void from_json(const nlohmann::json &j, StatBar &bar);

#endif
