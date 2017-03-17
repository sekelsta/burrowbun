#include <cassert>
#include "Tile.hh"

using namespace std;

// Constructor, based on the tile type
// This is basically a long list of the attributes of each tile type
Tile::Tile(TileType tileType, unsigned index) 
        : type(tileType), index(index) {
    // Make the pointer point to nothing
    sprite.texture = NULL;

    // These variables will usually be these values
    sprite.cols = 4;
    isSolid = true;
    isPlatform = false;
    isTiledLikePlatform = false;
    maxHealth = 1;
    erodeResist = -1;
    opacity = 64;

    // Set things to the right values
    switch(tileType) {
        case TileType::EMPTY : 
            isSolid = false;
            sprite.name = "";
            opacity = 0;
            break;
        case TileType::IMPASSABLE :
            isSolid = true;
            sprite.name = "";
            opacity = 0;
            break;
        case TileType::PLATFORM :
            sprite.name = "platform.png";
            opacity = 0;
            maxHealth = 2;
            isPlatform = true;
            isTiledLikePlatform = true;
            break;
        case TileType::DIRT :
            sprite.name = "dirt.png";
            maxHealth = 5;
            erodeResist = 10;
            break;
        case TileType::MUDSTONE :
            sprite.name = "mudstone.png";
            maxHealth = 10;
            erodeResist = 100;
            break;
        case TileType::PERIDOTITE :
            sprite.name = "peridotite.png";
            maxHealth = 3;
            break;
        case TileType::SANDSTONE :
            sprite.name = "sandstone.png";
            maxHealth = 8;
            erodeResist = 100;
            break;
        case TileType::RED_SANDSTONE :
            sprite.name = "red_sandstone.png";
            maxHealth = 8;
            erodeResist = 100;
            break;
        case TileType::DIAMOND :
            sprite.name = "diamond.png";
    }
}



