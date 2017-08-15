#ifndef BOULDER_HH
#define BOULDER_HH

#include <vector>
#include <set>
#include "Tile.hh"

/* Forward declare. */
struct Rect;

/* A Tile that moves. */
class Boulder: public Tile {
    /* How many ticks to wait before moving left or right. If this is 0,
    then this type of boulder never moves left of right. */
    int moveTicks;

    /* How many ticks to wait before falling. */
    int fallTicks;

    /* List of tile types which drop as an item when this type of boulder
    runs into them sideways. */
    std::set<TileType> tilesDestroyed;

    /* Same, but for falling. */
    std::set<TileType> tilesCrushed;

    /* List of tile types which this boulder will switch places with. */
    std::set<TileType> tilesDisplaced;

    /* Same, but for falling. */
    std::set<TileType> tilesSunk;

    /* The amount it moves in the x direction. Positive is to the right,
    negative is to the left, and 0 is for a boulder that doesn't ever move
    sideways. */
    int direction;

    /* Does it float or fall? */
    bool isFloating;

    /* Can they get in each other's way? */
    bool movesTogether;

    /* If someone stands on it while it moves, does it bring them? */
    bool carriesMovables;

    /* Convert a vector<int> to a vector<TileType> */
    static std::set<TileType> vectorConvert(const std::vector<int> &input);

    /* Try to fall one tile. Return true on success. */
    bool fall(Map &map, const Location &place) const;

    /* Try to move one tile. Return true on success. */
    bool move(Map &map, const Location &place, 
            std::vector<movable::Movable*> &movables) const;

    /* Try to move together. Return true on success. */
    bool moveTogether(Map &map, const Location &place,
            std::vector<movable::Movable*> &movables) const;

    /* Find which movables are on top and tell them to move. */
    void carryMovables(const Map &map, const Rect &boulderRect, 
            std::vector<movable::Movable*> &movables) const;

public:
    /* Constructor. */
    Boulder(TileType type);

    /* Look at the map and move, bringing movables along if required. 
    Return false if it didn't move and should therefore be removed from any
    lists of boulders to try to move. */
    virtual bool update(Map &map, Location place, 
            std::vector<movable::Movable*> &movables, 
            int tick) const;

    /* Look at the map and see if it can move, but don't do anything. */
    virtual bool canUpdate(const Map &map, const Location &place) const;
};

#endif
