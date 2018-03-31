#ifndef MAPHELPERS_HH
#define MAPHELPERS_HH

#include "Tile.hh"
#include "Light.hh"

/* An enum class to refer to the different layers of the map. */
enum class MapLayer {
    FOREGROUND,
    BACKGROUND,
    LIQUID,
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
    /*
    Possible other world types would be ones that focus more on certain biomes,
    such as:
    POLAR - tundra, taiga, ice caps, ice sheets, mammoth steppe, polar ocean
    TEMPERATE - shortgrass prairie, tallgrass prairie, steppe (Eurasian),
    woodlands, mediterranian / scrub, 
    TROPICAL - tropical rainforest, desert, savannah
    */
};

/* A class to talk about nonglobal (only part of the world) biomes.*/
enum class BiomeType {
    TUNDRA,
    TAIGA,
    GRASSLAND,
    WOODLAND, // Temperate deciduous forest
    JUNGLE, // Tropical rainforest
    SAVANNAH,
    SCRUB,
    DESERT,
    TALLGRASS,

    /* Wishlist: */
    ALPINE_TUNDRA,
    ALPINE_FOREST,

    SWAMP,
    MARSH,
    PEAT_SWAMP,
    BOG,
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

/* A struct to store information about the biome. */
struct BiomeInfo {
    BiomeType biome;
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
    Light sunlight;

    // Whether the light is actually set to the correct value
    bool isLightUpdated;
    bool lightRemoved;
    bool lightAdded;

    // Which rectangle of the spritesheet to draw
    uint8_t foregroundSprite;
    uint8_t backgroundSprite;

    // Constructor
    SpaceInfo() {
        foreground = TileType::EMPTY;
        background = TileType::EMPTY;
        isLightUpdated = false;
        lightRemoved = false;
        lightAdded = false;
        foregroundSprite = 0;
        backgroundSprite = 0;
    }

    static inline void fromSpritePlace(Location &place, uint8_t spritePlace) {
        place.x = spritePlace / 16;
        place.y = spritePlace % 16;
    }

    static inline uint8_t toSpritePlace(const Location &place) {
        return toSpritePlace(place.x, place.y);
    }

    static inline uint8_t toSpritePlace(int x, int y) {
        return 16 * x + y;
    } 

    static inline int getX(uint8_t spritePlace) {
        return spritePlace / 16;
    }

    static inline int getY(uint8_t spritePlace) {
        return spritePlace % 16;
    }
};



#endif
