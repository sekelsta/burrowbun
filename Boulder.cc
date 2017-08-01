#include <fstream>
#include <string>
#include "Boulder.hh"
#include "Tile.hh"
#include "Map.hh"
#include "MapHelpers.hh"
#include "json.hpp"

// For convenience
using json = nlohmann::json;

/* Convert a vector<int> to a vector<TileType>. */
vector<TileType> Boulder::vectorConvert(const vector<int> &input) {
    vector<TileType> output;
    for (unsigned int i = 0; i < input.size(); i++) {
        output.push_back((TileType)input[i]);
    }
    return output;
}

/* Constructor. */
Boulder::Boulder(TileType type) : Tile(type) {
    /* The name of the file where the initial values are stored. */
    string filename;
    /* The name of the folder in which the files are stored. */
    string prefix = "tiles/";
    switch(type) {
        case TileType::SAND :
            filename = "sand.json";
            break;
        default :
            /* We forgot a case aor something wierd is happening. */
            assert(false);
            break;
    }

    /* Put data in a json. */
    ifstream infile(prefix + filename);
    json j = json::parse(infile);

    /* Set the boulder's values to the json values. (The tile-but-not-boulder
    values should have already been set.) */
    moveTicks = j["moveTicks"];
    tilesDestroyed = vectorConvert(j["tilesDestroyed"].get<vector<int>>());
    tilesCrushed = vectorConvert(j["tilesCrushed"].get<vector<int>>());
    tilesDisplaced = vectorConvert(j["tilesDisplaced"].get<vector<int>>());
    tilesSunk = vectorConvert(j["tilesSunk"].get<vector<int>>());
    isFloating = j["isFloating"];
    carriesMovables = j["carriesMovables"];
}

/* Look at the map and move, bringing movables along if required. 
Return false if it didn't move and should therefore be removed from any
lists of boulders to try to move. */
bool Boulder::update(Map &map, Location place, vector<Movable*> &movables, int tick) {
// TODO
    return false;
}
