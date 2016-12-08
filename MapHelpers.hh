#ifndef MAPHELPERS_HH
#define MAPHELPERS_HH

// TODO: Actually create a Liquid class, not necessarily in this file. For now
// any Liquid* will just be set to NULL.
class Liquid;

// A struct for holding an x and a y coordinate.
struct Location {
    // The x and y values of the location
    int x;
    int y;
};

// A class to talk about directions. Down and West are smaller numbers.
enum class Direction {
    DOWN,
    UP,
    WEST,
    EAST
};

/* A class for passing a biome argument, to pick which pattern of 
world-generation to use. */
enum class WorldType {
    TEST,
    SMOLTEST,
    EARTH,
    MOON
};

/* A struct that holds information about a specific tile location on the map.
It has pointers to the foreground object (blocks, furniture, trees), the 
background object (walls), and the liquid. It also has ints for the health of
the foreground and background objects, where 'health' is roughly equivalent to
the number of times it can be hit with a pickax before it breaks. */
struct SpaceInfo {
    // The foreground and background objects
    Tile *foreground;
    Tile *background;
    
    // The liquid.
    Liquid *liquid;

    // Healths of the foreground and background
    int foregroundHealth;
    int backgroundHealth;
};

#endif
