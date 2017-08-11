#include <fstream>
#include <string>
#include "Boulder.hh"
#include "Tile.hh"
#include "Map.hh"
#include "MapHelpers.hh"
#include "Rect.hh"
#include "json.hpp"

// For convenience
using json = nlohmann::json;

/* Convert a vector<int> to a vector<TileType>. */
set<TileType> Boulder::vectorConvert(const vector<int> &input) {
    set<TileType> output;
    for (unsigned int i = 0; i < input.size(); i++) {
        output.insert((TileType)input[i]);
    }
    return output;
}

/* Check if it can fall one tile. If it can, do. */
bool Boulder::fall(Map &map, const Location &place, int ticks) const {
    /* If it isn't a type of boulder that can fall, return false. */
    if (isFloating) {
        return false;
    }

    /* If it hasn't been long enough for it to fall, return false. */
    if (ticks % fallTicks != 0) {
        return false;
    }

    TileType blocking = TileType::EMPTY;
    /* If there's a tile beneath, figure out which. */
    if (map.isOnMap(place.x, place.y - 1)) {
        /* If its path is blocked, it can't fall. */
        blocking = map.getTile(place.x, place.y - 1, place.layer) -> type;
    }
    
    /* If it can crush it, do so. */
    if (tilesCrushed.count(blocking)) {
        map.moveDown(place);
        return true;
    }
    else if (tilesSunk.count(blocking)) {
        map.displaceDown(place);
        return true;
    }

    return false;
}

/* Try to move one tile. Return true on success. */
bool Boulder::move(Map &map, const Location &place,
        vector<movable::Movable*> &movables, int tick) const {
    /* If it doesn't moves sideways at all ever, then it doesn't move.*/
    if (direction == 0) {
        return false;
    }
    /* Otherwise if it moves as a unit, this isn't the right function. */
    else if (movesTogether) {
        return false;
    }
    assert(moveTicks != 0);
    /* If it can't move yet, don't. */
    if (tick % moveTicks != 0) {
        return false;
    }

    TileType blocking = map.getTileType(place, direction, 0);
    if (tilesDestroyed.count(blocking)) {
        map.moveSideways(place, direction);
    }
    else if (tilesDisplaced.count(blocking)) {
        map.displaceSideways(place, direction);
    }
    else {
        return false;
    }

    /* We moved, so we should see if we need to carry movables. */
    if (carriesMovables) {
        /* Find the movables that are within one block above, 
        and add to their boulderSpeed. */
        Rect boulderRect;
        Rect movableRect;
        boulderRect.worldWidth = map.getWidth() * map.getTileWidth();
        movableRect.worldWidth = boulderRect.worldWidth;
        boulderRect.x = place.x * map.getTileWidth();
        boulderRect.y = place.y * map.getTileHeight();
        boulderRect.w = map.getTileWidth();
        boulderRect.h = 2 * map.getTileHeight();
        for (unsigned int i = 0; i < movables.size(); i++) {
            movableRect.x = movables[i] -> x;
            movableRect.y = movables[i] -> y;
            movableRect.w = movables[i] -> sprite.width;
            movableRect.h = movables[i] -> sprite.height;
            if (boulderRect.intersects(movableRect)) {
                movables[i] -> boulderSpeed += direction * map.getTileWidth();
            }
        }
    }

    return true;
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
    fallTicks = j["fallTicks"];
    tilesDestroyed = vectorConvert(j["tilesDestroyed"].get<vector<int>>());
    tilesCrushed = vectorConvert(j["tilesCrushed"].get<vector<int>>());
    tilesDisplaced = vectorConvert(j["tilesDisplaced"].get<vector<int>>());
    tilesSunk = vectorConvert(j["tilesSunk"].get<vector<int>>());
    direction = j["direction"];
    isFloating = j["isFloating"];
    movesTogether = j["movesTogether"];
    carriesMovables = j["carriesMovables"];
}

/* Look at the map and move, bringing movables along if required. 
Return false if it didn't move and should therefore be removed from any
lists of boulders to try to move. */
bool Boulder::update(Map &map, Location place, 
        vector<movable::Movable*> &movables, int tick) {
// TODO
    /* If it can fall, it should. */
    if (fall(map, place, tick)) {
        return true;
    }

    /* If it hasn't fallen this update, and wants to move sideways but not as
    a unit, it should do that. */
    if (move(map, place, movables, tick)) {
        return true;
    }
    /* If it hasn't fallen this update, and wants to move sideways as a unit, 
    it should do that. */
        /* Make sure we're the end of a row and no other places in the row
        are in the list of things to update. If the row stretched across the
        whole world with no end, just move the things on top. */

    return false;
}

bool Boulder::getNeedsUpdating() const {
    return true;
}
