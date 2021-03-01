#include <fstream>
#include <string>
#include "Boulder.hh"
#include "Tile.hh"
#include "Map.hh"
#include "MapHelpers.hh"
#include "../Rect.hh"
#include "../json.hh"
#include "../entity/DroppedItem.hh"

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
bool Boulder::fall(Map &map, const Location &place, 
        std::vector<DroppedItem*> &items) const {
    TileType blocking = map.getTileType(place, 0, -1);
    /* If it can crush it, do so. */
    if (tilesCrushed.count(blocking)) {
        map.moveTile(place, 0, -1, items);
        return true;
    }
    else if (tilesSunk.count(blocking)) {
        map.displaceTile(place, 0, -1);
        return true;
    }

    return false;
}

/* Try to move one tile. Return true on success. */
bool Boulder::move(Map &map, const Location &place, int direction,
        std::vector<DroppedItem*> &items) {
    /* If it has no preference for direction, it should move sideways if
    that will let it fall. */
    if (direction == 0) {
        if (!isSliding) {
            return false;
        }

        /* If it slides, pick a random direction (-1 or 1) to try to go. */
        int newDirection = (rand() % 2) * 2 - 1;
        assert(newDirection == -1 || newDirection == 1);
        move(map, place, newDirection, items);
        return true;
    }

    TileType blocking = map.getTileType(place, direction, 0);
    if (tilesDestroyed.count(blocking)) {
        map.moveTile(place, direction, 0, items);
    }
    else if (tilesDisplaced.count(blocking)) {
        map.displaceTile(place, direction, 0);
    }
    else {
        return false;
    }

    return true;
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
Boulder::Boulder(TileType type, string name_in) : Tile(type, name_in) {
    /* The name of the file where the initial values are stored. */
    std::string filename = getFilename();

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

void Boulder::setDirection(Map &map, const Location &place, int direction)
         const {
    assert(direction == 1 || direction == -1);
    /* Convert direction to 0 or 1. */
    direction = (direction + 1) / 2;
    Location spritePlace = map.getSprite(place);
    /* If direction = 1, spritePlace.y should be at least sprite.cols / 4. */
    spritePlace.x %= sprite.getCols() / 4;
    spritePlace.x += direction * (sprite.getCols() / 4);
    map.setSprite(place, spritePlace); 
}

/* Look at the map and move.
Return false if it didn't move and should therefore be removed from any
lists of boulders to try to move. */
bool Boulder::update(Map &map, Location place, 
        std::vector<DroppedItem*> &items, int tick) {
    /* If it can fall, it should. */
    if (fallTicks != 0 && (tick % fallTicks == 0) && !isFloating) {
        if (fall(map, place, items)) {
            return true;
        }
    }

    int direction = getDirection(map, place);

    /* If it hasn't fallen this update, and wants to move sideways, it should 
    do that. */
    if (moveTicks != 0 && (tick % moveTicks == 0)) {
        return move(map, place, direction, items);
    }
    return true;
}

bool Boulder::canUpdate(const Map &map, const Location &place) {
    int direction = getDirection(map, place);
    return canUpdate(map, place, direction);
}

/* Figure out the direction to go from the tile sprite. */
int Boulder::getDirection(const Map &map, const Location &place) const {
    if (!isMoving) {
        return 0; 
    }
    Location spritePlace = map.getSprite(place);
    return (spritePlace.x < sprite.getCols() / 4) ? -1 : 1;
}

