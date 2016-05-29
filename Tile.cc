#include "Tile.hh"

using namespace std;


// Constructor, based on the tile type
// This is basically a long list of the attributes of each tile type
Tile::Tile(TileType tileType) {
    type = tileType;
    // Set Height and width. It's probably best if this is the same
    // for all types of tiles. Maybe it should be a field of Map.
    height = 20;
    width = 20;
    // These variables will usually be these values
    isSolid = true;
    isPlatform = false;
    erodeResist = -1;
    maxTorque = 0;
    maxPressure = -1;
    // Set things to the right values
    switch(tileType) {
        case TileType::EMPTY : 
            sprite = "empty.png";
            isSolid = false;
            mass = 0;
            break;
        case TileType::PLATFORM :
            sprite = "platform.png";
            mass = 0;
            isPlatform = true;
            break;
        case TileType::DIRT :
            sprite = "dirt.png";
            mass = 5;
            erodeResist = 10;
            maxTorque = 1;
            maxPressure = 500;
            pressureMetamorph = TileType::STONE;
            break;
        case TileType::STONE :
            sprite = "stone.png";
            mass = 10;
            erodeResist = 100;
            maxTorque = 50;
            maxPressure = 10000;
            pressureMetamorph = TileType::MAGMA;
            break;
        case TileType::MAGMA :
            sprite = "magma.png";
            mass = 15;
            maxTorque = 10;
            break;
        case TileType::SANDSTONE :
            sprite = "sandstone.png";
            mass = 10;
            erodeResist = 100;
            maxTorque = 50;
            // TODO: pressure metamorph is quartzite
            break;
        case TileType::MUDSTONE :
            sprite = "mudstone.png";
            mass = 10;
            erodeResist = 100;
            maxTorque = 50;
            break;
    }
}



