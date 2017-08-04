#ifndef MAPHELPERS_HH
#define MAPHELPERS_HH

#include "Tile.hh"
#include "Light.hh"

/* An enum class to refer to the different layers of the map. */
enum class MapLayer {
    FOREGROUND,
    BACKGROUND
};

/* A struct for holding an x and a y coordinate, plus a layer type. */
struct Location {
    int x;
    int y;
    MapLayer layer;

    /* Return true if every value is the same. */
    bool operator==(const Location &location);
};

struct TileHealth {
    Location place;
    int health;
    int lastUpdated;
};

/* A class for passing a global-biome argument, to pick which pattern of 
world-generation to use. */
enum class WorldType {
    TEST,
    SMOLTEST,
    EARTH,
    MOON
};

/* A class to talk about nonglobal (only part of the world) biomes.*/
enum class BiomeType {
    ARCTIC_TUNDRA,
    ALPINE_TUNDRA,
    TAIGA,
    ALPINE_FOREST,
    GRASSLAND,
    WOODLAND, // Temperate forest
    DEEP_WOODS,
    JUNGLE, // Either tropical rainforest or monsoon forest?
    SAVANNAH,
    DESERT,
    SWAMP,
    BOG,
    SKY,
    UNDERGROUND,
    // Might not be implemented for a good long while:
    COASTAL,
    REEF,
    PELAGIC,
    BENTHIC
};

/* Whether that part of the world is particularily magical and what type of
   magic. Also whether things that die there become undead. */
enum class Surroundings {
    GOOD,
    NONE,
    EVIL,
    REANIMATING
};

/* A struct that holds information about a specific tile location on the map.
It has pointers to the foreground object (blocks, furniture, trees), the 
background object (walls), and the liquid. It also has ints for the health of
the foreground and background objects, where 'health' is roughly equivalent to
the number of times it can be hit with a pickax before it breaks. */
struct SpaceInfo {
    // The foreground and background objects
    // TODO: just store the tiletype here. Then have an array of tile objects
    // indexed by tiletype to refer to.
    Tile *foreground;
    Tile *background;

    // How well-lit this tile is, and from where
    Light light;

    // Whether the light is actually set to the correct value
    bool isLightUpdated;

    // Which rectangle of the spritesheet to draw
    // spriteRow should be based on which surrounding tiles are empty,
    // spriteCol should be random
    Location spritePlace;
};

#endif
