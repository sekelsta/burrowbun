#include <cassert>
#include "Tile.hh"

using namespace std;

// All the access functions
bool Tile::getIsPlatform() const {
    return isPlatform;
}

bool Tile::getIsSolid() const {
    return isSolid;
}

int Tile::getOpacity() const {
    return opacity;
}

int Tile::getMaxHealth() const {
    return maxHealth;
}

/* Deal damage to whatever is overlapping this, and stop it if this tile is 
solid. */
void Tile::dealOverlapDamage(Movable &movable) const {
    movable.takeDamage(overlapDamage, 0);
}

// Constructor, based on the tile type
// This is basically a long list of the attributes of each tile type
Tile::Tile(TileType tileType, unsigned index) 
        : type(tileType), index(index) {
    // Make the pointer point to nothing
    sprite.texture = NULL;

    // These variables will usually be these values
    sprite.rows = 16;
    sprite.cols = 4;
    isSolid = true;
    isPlatform = false;
    overlapDamage = 0;
    maxHealth = 1;
    opacity = 64;

    // Set things to the right values
    switch(tileType) {
        case TileType::EMPTY : 
            isSolid = false;
            sprite.name = "";
            opacity = 0;
            break;
        case TileType::DIRT :
            sprite.name = "dirt.png";
            maxHealth = 5;
            break;
        case TileType::HUMUS :
            sprite.name = "humus.png";
            maxHealth = 3;
            break;
        case TileType::SAND : 
            sprite.name = "sand.png";
            maxHealth = 3;
            overlapDamage = 10;
            break;
        case TileType::CLAY :
            sprite.name = "clay.png";
            maxHealth = 4;
            break;
        case TileType::CALCAREOUS_OOZE :
            sprite.name = "calcareous_ooze.png";
            maxHealth = 2;
            break;
        case TileType::SNOW :
            sprite.name = "snow.png";
            maxHealth = 3;
            break;
        case TileType::ICE :
            sprite.name = "ice.png";
            maxHealth = 6;
            break;
        case TileType::STONE :
            sprite.name = "stone.png";
            maxHealth = 8;
            break;
        case TileType::GRANITE :
            sprite.name = "granite.png";
            maxHealth = 8;
            break;
        case TileType::BASALT : 
            sprite.name = "basalt.png";
            maxHealth = 8;
            break;
        case TileType::LIMESTONE : 
            sprite.name = "limestone.png";
            maxHealth = 7;
            break;
        case TileType::MUDSTONE :
            sprite.name = "mudstone.png";
            maxHealth = 10;
            break;
        case TileType::PERIDOTITE :
            sprite.name = "peridotite.png";
            maxHealth = 3;
            break;
        case TileType::SANDSTONE :
            sprite.name = "sandstone.png";
            maxHealth = 8;
            break;
        case TileType::RED_SANDSTONE :
            sprite.name = "red_sandstone.png";
            maxHealth = 8;
            break;
        case TileType::PLATFORM :
            sprite.name = "platform.png";
            sprite.cols = 1;
            opacity = 0;
            maxHealth = 2;
            isPlatform = true;
            isSolid = false;
            break;
        case TileType::LUMBER :
            sprite.name = "lumber.png";
            sprite.cols = 1;
            maxHealth = 8;
            break;
        case TileType::RED_BRICK :
            sprite.name = "red_brick.png";
            sprite.cols = 1;
            maxHealth = 12;
            break;
        case TileType::GRAY_BRICK :
            sprite.name = "gray_brick.png";
            sprite.cols = 1;
            maxHealth = 12;
            break;
        case TileType::DARK_BRICK : 
            sprite.name = "dark_brick.png";
            sprite.cols = 1;
            maxHealth = 20;
            break;
    }
}



