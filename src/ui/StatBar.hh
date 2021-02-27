#ifndef STATBAR_HH
#define STATBAR_HH

#include <iostream>
#include <SDL2/SDL.h>
#include "../Light.hh"
#include "../Stat.hh"
#include "../render/Sprite.hh"



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

    // What color to draw the different rects
    Light fullColor;
    Light partColor;
    Light emptyColor;

    /* The sprite to go over the color. */
    Sprite overlay;

private:
    // Translate from portion of max health to portion of bar filled
    int convert(double newValue, double maxStat) {
        if (maxStat != 0) {
            double fraction = newValue / maxStat;
            return fraction * totalWidth;
        }
        else {
            return 0;
        }
    }

public:
    // Set the amount of the stat
    void setFull(double newValue, double maxStat) {
        assert(0 <= newValue);
        assert(newValue <= maxStat);

        full = convert(newValue, maxStat);
    }

    // Set the temporary cap (which prevents the stat from regenerating 
    // completely)
    void setPart(double newValue, double maxStat) {
        assert(0 <= newValue);
        assert(newValue <= maxStat);

        part = convert(newValue, maxStat);
    }

    /* Set the values based on stat. */
    void update(const Stat &stat) {
        setFull(stat.full, stat.maxStat);
        setPart(stat.part, stat.maxStat);
        assert(full <= part);
    }

    /* Render itself. */
    void render();
};

/* Get a statbar from a json. */
void from_json(const nlohmann::json &j, StatBar &bar);

#endif
