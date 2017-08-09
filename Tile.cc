#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include "Tile.hh"
#include "Map.hh"
#include "Movable.hh"
#include "json.hpp"

// For convenience
using json = nlohmann::json;

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
void Tile::dealOverlapDamage(movable::Movable &movable) const {
    movable.takeDamage(overlapDamage, 0);
}

/* Change the map in whatever way needs doing. */
bool Tile::update(Map &map, Location place, 
        vector<movable::Movable*> &movables, int tick) {
    return false;
}

// Constructor, based on the tile type
// This is basically a long list of the attributes of each tile type
Tile::Tile(TileType tileType) 
        : type(tileType) {
    string filename;
    string prefix = "tiles/";

    /* Figure out the right json file to use. */
    switch(tileType) {
        case TileType::EMPTY : 
            filename = "empty.json";
            break;
        case TileType::DIRT :
            filename = "dirt.json";
            break;
        case TileType::HUMUS :
            filename = "humus.json";
            break;
        case TileType::SAND : 
            filename = "sand.json";
            break;
        case TileType::CLAY :
            filename = "clay.json";
            break;
        case TileType::CALCAREOUS_OOZE :
            filename = "calcareous_ooze.json";
            break;
        case TileType::SNOW :
            filename = "snow.json";
            break;
        case TileType::ICE :
            filename = "ice.json";
            break;
        case TileType::STONE :
            filename = "stone.json";
            break;
        case TileType::GRANITE :
            filename = "granite.json";
            break;
        case TileType::BASALT : 
            filename = "basalt.json";
            break;
        case TileType::LIMESTONE : 
            filename = "limestone.json";
            break;
        case TileType::MUDSTONE :
            filename = "mudstone.json";
            break;
        case TileType::PERIDOTITE :
            filename = "peridotite.json";
            break;
        case TileType::SANDSTONE :
            filename = "sandstone.json";
            break;
        case TileType::RED_SANDSTONE :
            filename = "red_sandstone.json";
            break;
        case TileType::PLATFORM :
            filename = "platform.json";
            break;
        case TileType::LUMBER :
            filename = "lumber.json";
            break;
        case TileType::RED_BRICK :
            filename = "red_brick.json";
            break;
        case TileType::GRAY_BRICK :
            filename = "gray_brick.json";
            break;
        case TileType::DARK_BRICK : 
            filename = "dark_brick.json";
            break;
    }

    /* Put data in json. */
    ifstream infile(prefix + filename);
    json j = json::parse(infile);
    /* Set each of this tile's non-const values equal to the json's values. */
    sprite = j["sprite"].get<Sprite>();
    isSolid = j["isSolid"];
    isPlatform = j["isPlatform"];
    overlapDamage = j["overlapDamage"];
    maxHealth = j["maxHealth"];
    opacity = j["opacity"];
}

/* Virtual destructor. */
Tile::~Tile() {}

/* Whether the tile will ever need to call its update function. */
bool Tile::getNeedsUpdating() const {
    return false;
}
