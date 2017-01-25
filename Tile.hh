#ifndef TILE_HH
#define TILE_HH

#include <string>

// Forward declare
struct SDL_Texture;

using namespace std;



// A class for keeping track of which tiles there are
enum class TileType {
    EMPTY,
    IMPASSABLE,
    DIRT,
    MUDSTONE,
    SANDSTONE,
    RED_SANDSTONE,
    PERIDOTITE,
    DIAMOND,
    PLATFORM,
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

    // The name of the image file
    std::string sprite;
    // The number of variations of the tile that sprites exist of
    int maxSpriteCol;

    // The texture made from that image file
    SDL_Texture *texture;

    // Variables for how it interacts with the players
    bool isPlatform; // Whether players collide with the underside
    bool isTiledLikePlatform; // Platforms look at the tiles next to them and
                              // diagonal when deciding which sprite to use
    bool isSolid;

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
