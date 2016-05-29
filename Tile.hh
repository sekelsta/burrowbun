#include <string>
#include <cassert>

using namespace std;



// A class for keeping track of which tiles there are
enum class TileType {
    MAGMA,
    EMPTY,
    DIRT,
    STONE,
    PLATFORM,
    SANDSTONE,
    MUDSTONE
};

/* A class to make tiles based on their type, and storre their infos. */
class Tile {
public:
    // The name of this type of tile
    TileType type;

    // The height and width of the tile when displayed in game, in pixels
    int height, width;

    // The name of the image file
    std::string sprite;

    // Variables for how it interacts with the players
    bool isSolid;    // Whether players can go through it
    bool isPlatform; // Whether players collide with the underside

    // Variables to use in map generation and upkeep
    // In these ones, -1 means infinity
    float mass;        // How heavy one tile of it is
    float erodeResist;  // How hard it is to erode
    float maxTorque;   // How much it can take before it falls over
    float maxPressure; // How much pressure before it metamorphoses
    // TODO: implement heat-based metamorphism
    TileType pressureMetamorph; // What it becomes

    // Constructor, based on the tile type
    // Also the only actual method Tiles have
    Tile(TileType tileType);
};
