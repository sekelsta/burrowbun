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
    passage = 1;
    isPlatform = false;
    maxHealth = 1;
    erodeResist = -1;
    maxPressure = -1;

    // Set things to the right values
    switch(tileType) {
        case TileType::NONE :
            break;
        case TileType::EMPTY : 
            sprite = "empty.png";
            mass = 0;
            break;
        case TileType::PLATFORM :
            sprite = "platform.png";
            mass = 0;
            maxHealth = 2;
            isPlatform = true;
            break;
        case TileType::DIRT :
            sprite = "dirt.png";
            mass = 5;
            maxHealth = 5;
            erodeResist = 10;
            maxPressure = 500;
            pressureMetamorph = TileType::STONE;
            break;
        case TileType::STONE :
            sprite = "stone.png";
            mass = 10;
            maxHealth = 10;
            erodeResist = 100;
            maxPressure = 10000;
            pressureMetamorph = TileType::MAGMA;
            break;
        case TileType::MAGMA :
            sprite = "magma.png";
            mass = 15;
            maxHealth = 3;
            break;
        case TileType::SANDSTONE :
            sprite = "sandstone.png";
            mass = 10;
            maxHealth = 8;
            erodeResist = 100;
            // TODO: pressure metamorph is quartzite
            break;
        case TileType::MUDSTONE :
            sprite = "mudstone.png";
            mass = 10;
            maxHealth = 8;
            erodeResist = 100;
            break;
    }
}



