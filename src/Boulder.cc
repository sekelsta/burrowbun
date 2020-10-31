#include <fstream>
#include <string>
#include "Boulder.hh"
#include "Tile.hh"
#include "Map.hh"
#include "MapHelpers.hh"
#include "Rect.hh"
#include "json.hh"

#define BOULDER_CARRY_HEIGHT 1.5

// For convenience
using json = nlohmann::json;

using namespace std;

/* Convert a vector<int> to a vector<TileType>. */
std::set<TileType> Boulder::vectorConvert(const std::vector<int> &input) {
    std::set<TileType> output;
    for (unsigned int i = 0; i < input.size(); i++) {
        output.insert((TileType)input[i]);
    }
    return output;
}

/* Check if it can fall one tile. If it can, do. */
bool Boulder::fall(Map &map, const Location &place) const {
    TileType blocking = map.getTileType(place, 0, -1);
    /* If it can crush it, do so. */
    if (tilesCrushed.count(blocking)) {
        //map.moveTile(place, 0, -1);
        return true;
    }
    else if (tilesSunk.count(blocking)) {
        //map.displaceTile(place, 0, -1);
        return true;
    }

    return false;
}

bool Boulder::canUpdate(const Map &map, const Location &place, 
        int direction) const {
    TileType below = map.getTileType(place, 0, -1);
    if ((!isFloating) 
            && (tilesCrushed.count(below) || tilesSunk.count(below))) {
        return true;
    }
    if (direction == 0) {
        if (!isSliding) {
            return false;
        }
        /* Check if it can go in either direction. */
        return canUpdate(map, place, 1) || canUpdate(map, place, -1);;
    }
    TileType ahead = map.getTileType(place, direction, 0);
    if (tilesDestroyed.count(ahead) || tilesDisplaced.count(ahead)) {
        return true;
    }
    return false;

}

/* Constructor. */
Boulder::Boulder(TileType type, string path) : Tile(type, path) {
    /* The name of the file where the initial values are stored. */
    std::string filename = path + getFilename(type);

    /* Put data in a json. */
    std::ifstream infile(filename);
    /* Check that file was opened successfully. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);

    /* Set the boulder's values to the json values. (The tile-but-not-boulder
    values should have already been set.) */
    moveTicks = j["moveTicks"];
    fallTicks = j["fallTicks"];
    tilesDestroyed = vectorConvert(j["tilesDestroyed"].get<std::vector<int>>());
    tilesCrushed = vectorConvert(j["tilesCrushed"].get<std::vector<int>>());
    tilesDisplaced = vectorConvert(j["tilesDisplaced"].get<std::vector<int>>());
    tilesSunk = vectorConvert(j["tilesSunk"].get<std::vector<int>>());
    isMoving = j["isMoving"];
    isFloating = j["isFloating"];
    isSliding = j["isSliding"];
}

/* Look at the map and move.
Return false if it didn't move and should therefore be removed from any
lists of boulders to try to move. */
bool Boulder::update(Map &map, Location place, int tick) {
    std::cout << "This is getting called\n";
    /* If it can fall, it should. */
    if (fallTicks != 0 && (tick % fallTicks == 0) && !isFloating) {
        if (fall(map, place)) {
            return true;
        }
    }
    return true;
}

bool Boulder::canUpdate(const Map &map, const Location &place) {
    return canUpdate(map, place, 0);
}

