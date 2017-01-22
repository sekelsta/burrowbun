#include <cassert>
#include "Tile.hh"

using namespace std;

// Constructor, based on the tile type
// This is basically a long list of the attributes of each tile type
Tile::Tile(TileType tileType, unsigned index) 
        : type(tileType), index(index) {
    // Make the pointer point to nothing
    texture = NULL;

    // These variables will usually be these values
    maxSpriteCol = 4;
    isSolid = true;
    isPlatform = false;
    isTiledLikePlatform = false;
    maxHealth = 1;
    erodeResist = -1;

    // Set things to the right values
    switch(tileType) {
        case TileType::NONE :
            break;
        case TileType::EMPTY : 
            isSolid = false;
            sprite = "";
            break;
        case TileType::IMPASSABLE :
            isSolid = true;
            sprite = "";
        case TileType::PLATFORM :
            sprite = "platform.png";
            maxHealth = 2;
            isPlatform = true;
            isTiledLikePlatform = true;
            break;
        case TileType::DIRT :
            sprite = "dirt.png";
            maxHealth = 5;
            erodeResist = 10;
            break;
        case TileType::MUDSTONE :
            sprite = "mudstone.png";
            maxHealth = 10;
            erodeResist = 100;
            break;
        case TileType::PERIDOTITE :
            sprite = "peridotite.png";
            maxHealth = 3;
            break;
        case TileType::SANDSTONE :
            sprite = "sandstone.png";
            maxHealth = 8;
            erodeResist = 100;
            break;
        case TileType::RED_SANDSTONE :
            sprite = "red_sandstone.png";
            maxHealth = 8;
            erodeResist = 100;
            break;
        case TileType::DIAMOND :
            sprite = "diamond.png";
    }
}



