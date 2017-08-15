#include <fstream>
#include <string>
#include "Boulder.hh"
#include "Tile.hh"
#include "Map.hh"
#include "MapHelpers.hh"
#include "Rect.hh"
#include "json.hpp"

#define BOULDER_CARRY_HEIGHT 1.5

// For convenience
using json = nlohmann::json;

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
        std::vector<movable::Movable*> &movables) const {
    /* If it doesn't moves sideways at all ever, then it doesn't move.*/
    if (direction == 0) {
        return false;
    }

   /* Otherwise if it moves as a unit, this isn't the right function. */
    if (movesTogether) {
        return moveTogether(map, place, movables);
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
        boulderRect.worldWidth = map.getWidth() * map.getTileWidth();
        boulderRect.x = place.x * map.getTileWidth();
        boulderRect.y = place.y * map.getTileHeight();
        boulderRect.w = map.getTileWidth();
        boulderRect.h = BOULDER_CARRY_HEIGHT * map.getTileHeight();
        carryMovables(map, boulderRect, movables);
   }

    return true;
}

bool Boulder::moveTogether(Map &map, const Location &place, 
        std::vector<movable::Movable*> &movables) const {
    /* Make sure we're the end of a row and no other places in the row
    are in the list of things to update. If the row stretched across the
    whole world with no end, just move the things on top. If there's a 
    gap one tile wide, treat the whole thing as one continuous row. */
    /* This isn't going to work if direction is something other than
    1 or -1. */
    assert(direction == 1 || direction == -1);
    TileType oneBehind = map.getTileType(place, -1 * direction, 0);
    TileType twoBehind = map.getTileType(place, -2 * direction, 0);
    assert(map.getTileType(place, 0, 0) == type);
    int distToLast = 0;
    while ((oneBehind == type || twoBehind == type
                || (tilesDisplaced.count(oneBehind) && twoBehind == type)
                || (tilesCrushed.count(oneBehind) && twoBehind == type)) 
            && abs(distToLast) != map.getWidth()) {
        distToLast -= direction;
        oneBehind = twoBehind;
        twoBehind = map.getTileType(place, distToLast - 2 * direction, 0);    
    }
    if (distToLast == map.getWidth()) {
        /* Just sit here and pretend everything's okay. */
        // TODO: actually do something
        return false;
    }
    else if (distToLast != 0) {
        /* Make sure the last cloud is in the update list and return false. */
        map.addToUpdate(place.x + distToLast, place.y, place.layer);
        return false;
    }

    /* So now we know we're the last block. Time to move everyone. */
    int forwards = 0;
    int unmoved = 0;
    TileType current = type;
    TileType ahead = map.getTileType(place, direction, 0);
    while ((current == type || ahead == type) 
            && abs(forwards) != map.getWidth()) {
        forwards += direction;
        if (ahead != type) {
            Location toMove = place;
            toMove.x += unmoved;
            if (tilesDestroyed.count(ahead)) {
                map.moveSideways(toMove, forwards - unmoved);
                unmoved = forwards + direction;
            }
            else if (tilesDisplaced.count(ahead)) {
                map.displaceSideways(toMove, forwards - unmoved);
                unmoved = forwards + direction;
            }
            else {
                /* Our group of clouds is stuck behind a block but there's more
                clouds ahead. Make sure the last of those ones is being 
                updated. */
                map.addToUpdate(place.x + forwards + direction, 
                        place.y, place.layer);
                return unmoved;
            }
        }
        current = ahead;
        ahead = map.getTileType(place, forwards + direction, 0);
    }
  
    if (carriesMovables) { 
        Rect boulderRect;
        boulderRect.worldWidth = map.getWidth() * map.getTileWidth();
        boulderRect.y = place.y * map.getTileHeight();
        boulderRect.h = BOULDER_CARRY_HEIGHT * map.getTileHeight();
        boulderRect.w = abs(forwards) * map.getTileWidth();
        /* Doesn't matter that boulderRect.x could be negative. */
        boulderRect.x = std::min(place.x, place.x + forwards);
        boulderRect.x *= map.getTileWidth();
        carryMovables(map, boulderRect, movables);
    }

    return unmoved;
}

void Boulder::carryMovables(const Map &map, const Rect &boulderRect, 
        std::vector<movable::Movable*> &movables) const {
    Rect movableRect;
    movableRect.worldWidth = boulderRect.worldWidth;
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

/* Constructor. */
Boulder::Boulder(TileType type) : Tile(type) {
    /* The name of the file where the initial values are stored. */
    std::string filename;
    /* The name of the folder in which the files are stored. */
    std::string prefix = "tiles/";
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
    std::ifstream infile(prefix + filename);
    json j = json::parse(infile);

    /* Set the boulder's values to the json values. (The tile-but-not-boulder
    values should have already been set.) */
    moveTicks = j["moveTicks"];
    fallTicks = j["fallTicks"];
    tilesDestroyed = vectorConvert(j["tilesDestroyed"].get<std::vector<int>>());
    tilesCrushed = vectorConvert(j["tilesCrushed"].get<std::vector<int>>());
    tilesDisplaced = vectorConvert(j["tilesDisplaced"].get<std::vector<int>>());
    tilesSunk = vectorConvert(j["tilesSunk"].get<std::vector<int>>());
    direction = j["direction"];
    isFloating = j["isFloating"];
    movesTogether = j["movesTogether"];
    carriesMovables = j["carriesMovables"];
}

/* Look at the map and move, bringing movables along if required. 
Return false if it didn't move and should therefore be removed from any
lists of boulders to try to move. */
bool Boulder::update(Map &map, Location place, 
        std::vector<movable::Movable*> &movables, int tick) const {
    /* If it can fall, it should. */
    if (fallTicks != 0 && (tick % fallTicks == 0) && !isFloating) {
        if (fall(map, place)) {
            return true;
        }
    }

    /* If it hasn't fallen this update, and wants to move sideways, it should 
    do that. */
    if (moveTicks != 0 && (tick % moveTicks == 0)) {
        return move(map, place, movables);
    }
    return true;
}

bool Boulder::canUpdate(const Map &map, const Location &place) const {
    TileType below = map.getTileType(place, 0, -1);
    if ((!isFloating) 
            && (tilesCrushed.count(below) || tilesSunk.count(below))) {
        return true;
    }
    if (direction == 0) {
        return false;
    }
    TileType ahead = map.getTileType(place, direction, 0);
    if (!movesTogether 
            && (tilesDestroyed.count(ahead) || tilesDisplaced.count(ahead))) {
        return true;
    }
    TileType behind = map.getTileType(place, -1 * direction, 0);
    return movesTogether && behind != type;
}
