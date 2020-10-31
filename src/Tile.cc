#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include "Tile.hh"
#include "Map.hh"
#include "json.hh"
#include "filepaths.hh"

#define TILE_ANIMATION_DELAY 4

// For convenience
using json = nlohmann::json;

using namespace std;

/* Get the filename of the json for this tiletype. */
std::string Tile::getFilename(TileType tileType) {
    string filename;
    string prefix = "tiles/";

    /* Figure out the right json file to use. */
    switch(tileType) {
        case TileType::EMPTY : 
            filename = "empty.json";
            break;
        case TileType::WATER :
            filename = "water.json";
            break;
        case TileType::DIRT :
            filename = "dirt.json";
            break;
        case TileType::TOPSOIL :
            filename = "topsoil.json";
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
        case TileType::GLASS :
            filename = "glass.json";
            break;
        case TileType::GLOWSTONE :
            filename = "glowstone.json";
            break;
        case TileType::TORCH :
            filename = "torch.json";
            break;
        case TileType::SAND :
            filename = "sand.json";
            break;
        case TileType::MUD :
            filename = "mud.json";
            break;
        case TileType::CLOUD :
            filename = "cloud.json";
            break;
        case TileType::BOULDER :
            filename = "boulder.json";
            break;
        case TileType::GLACIER :
            filename = "glacier.json";
            break;
    }

    return prefix + filename;
}

// All the access functions
bool Tile::getIsPlatform() const {
    return isPlatform;
}

bool Tile::getIsSolid() const {
    return isSolid;
}



int Tile::getMaxHealth() const {
    return maxHealth;
}

/* Change the map in whatever way needs doing. */
bool Tile::update(Map &map, Location place, int tick) {
    return false;
}

// Constructor, based on the tile type
Tile::Tile(TileType tileType, string path) 
        : type(tileType) {
    string filename = path + getFilename(tileType);
    /* Put data in json. */
    ifstream infile(filename);
    /* Check that file was opened successfully. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);
    /* Set each of this tile's non-const values equal to the json's values. */
    color = j["color"].get<Light>();
    isSolid = j["isSolid"];
    isPlatform = j["isPlatform"];
    canBackground = j["canBackground"];
    waterBreaks = j["waterBreaks"];
    isAnimated = j["isAnimated"];
    emitted = j["emitted"].get<Light>();
    absorbed = {j["absorbed"]["r"], j["absorbed"]["g"], 
        j["absorbed"]["b"], j["absorbed"]["a"]};
    isSky = j["isSky"];
    maxHealth = j["maxHealth"];
    tier = j["tier"];
    int edgeInt = j["edgeType"];
    edgeType = (EdgeType)edgeInt;

    assert(absorbed.r >= 1.0);
    assert(absorbed.g >= 1.0);
    assert(absorbed.b >= 1.0);
    /* Not technically required but I can imagine mistakenly making it so much
    more easily than I can imagine actually wanting it to be the case. */
    assert(absorbed.a < 1.0);
}

/* Virtual destructor. */
Tile::~Tile() {}

/* Whether the tile will ever need to call its update function. */
bool Tile::canUpdate(const Map &map, const Location &place) {
    return isAnimated;
}
