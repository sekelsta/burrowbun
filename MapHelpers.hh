#ifndef MAPHELPERS_HH
#define MAPHELPERS_HH

#include "Tile.hh"
#include "Light.hh"

/* An enum class to refer to the different layers of the map. */
enum class MapLayer {
    FOREGROUND,
    BACKGROUND,
    NONE
};

/* A struct for holding an x and a y coordinate, plus a layer type. */
struct Location {
    int x;
    int y;
    MapLayer layer;

    /* Return true if every value is the same. */
    bool operator==(const Location &location) const;

    /* Return false if the value is the same. */
    bool operator!=(const Location &location) const;

    /* Return true if the first is less than the second. */
    bool operator<(const Location &location) const;
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
    TileType foreground;
    TileType background;

    // How well-lit this tile is, and from where
    Light light;

    // Whether the light is actually set to the correct value
    bool isLightUpdated;

    // Which rectangle of the spritesheet to draw
    uint8_t foregroundSprite;
    uint8_t backgroundSprite;

    static inline void fromSpritePlace(Location &place, Uint8 spritePlace) {
        place.x = spritePlace % 16;
        place.y = spritePlace / 16;
    }

    static inline Uint8 toSpritePlace(const Location &place) {
        return toSpritePlace(place.x, place.y);
    }

    static inline Uint8 toSpritePlace(int x, int y) {
        return 16 * y + x;
    } 
};



#endif
