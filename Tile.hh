#ifndef TILE_HH
#define TILE_HH

#include "Sprite.hh"

using namespace std;



// A class for keeping track of which tiles there are
enum class TileType {
    EMPTY,
    DIRT,
    HUMUS,
    SAND,
    CLAY,
    CALCAREOUS_OOZE,
    SNOW,
    ICE,
    STONE,
    GRANITE,
    BASALT,
    LIMESTONE,
    MUDSTONE,
    PERIDOTITE,
    SANDSTONE,
    RED_SANDSTONE,
    PLATFORM,
    LUMBER,
    RED_BRICK,
    GRAY_BRICK,
    DARK_BRICK
};

/* A class to make tiles based on their type, and store their infos. */
// Maybe since maps are filled with pointers to the same tile, everything
// should be constant
class Tile {
public:
    // The name of this type of tile
    const TileType type;

    // This tile's position in the map's vector of tile pointers
    unsigned index;

    // Information about the sprite
    // Tile spritesheets use rows for the different versions that depend on
    // whether each side is next to air, and the cols are the different 
    // variations.
    Sprite sprite;

    // Variables for how it interacts with the players
    bool isPlatform; // Whether players collide with the underside
    bool isSolid;

    /* For lighting. Tiles with 0 opacity are completely permeable to light.*/
    int opacity;

    // Basically the number of hits with a pickax to break it
    int maxHealth;

    // Variables to use in map generation and upkeep
    // In these ones, -1 means infinity
    double erodeResist;  // How hard it is to erode

    // Constructor, based on the tile type
    // Also the only actual method Tiles have
    Tile(TileType tileType, unsigned index);
};

#endif
