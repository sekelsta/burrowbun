#ifndef TILE_HH
#define TILE_HH

#include "Sprite.hh"
#include "Movable.hh"

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
// should be constant?
class Tile {
    // Collision-related varables
    /* Whether the tile is a platform. Palyers only colide with the tops side
    of platforms. */
    bool isPlatform;
    /* Whether players can pass through the tile. */
    bool isSolid;
    /* How much damage the player takes from sharing space with this tile. */
    int overlapDamage;

    // Display-related variables
    /* Tiles with 0 opacity are completely permeable to light. */
    int opacity;

    // Mining-related variables
    /* How much health the tile has determines how many hits it can take from
    a pickax before it breaks. */
    int maxHealth;

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
    bool getIsPlatform() const;
    bool getIsSolid() const;
    void dealOverlapDamage(Movable &movable) const;

    /* For lighting. Tiles with 0 opacity are completely permeable to light.*/
    int getOpacity() const;

    // Basically the number of hits with a pickax to break it
    int getMaxHealth() const;

    // Constructor, based on the tile type
    Tile(TileType tileType, unsigned index);
};

#endif
