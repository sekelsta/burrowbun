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

    /* Whether it's moving sideways in a particular direction. */
    bool isMoving;

    /* Does it float or fall? */
    bool isFloating;

    /* If it falls, does it stay in neat stacks or go sideways to fall
    more? */
    bool isSliding;

    /* Can they get in each other's way? */
    bool movesTogether;

    /* If someone stands on it while it moves, does it bring them? */
    bool carriesMovables;

    /* Convert a vector<int> to a vector<TileType> */
    static std::set<TileType> vectorConvert(const std::vector<int> &input);

    /* Try to fall one tile. Return true on success. */
    bool fall(Map &map, const Location &place) const;

    /* Try to move one tile. Return true on success. */
    bool move(Map &map, const Location &place, int direction, 
            std::vector<movable::Movable*> &movables);

    /* Try to move together. Return true on success. */
    bool moveTogether(Map &map, const Location &place, int direction,
            std::vector<movable::Movable*> &movables) const;

    /* Find which movables are on top and tell them to move. */
    void carryMovables(const Map &map, const Rect &boulderRect, 
            int direction, std::vector<movable::Movable*> &movables) const;

    bool canUpdate(const Map &map, const Location &place, 
            int direction) const;

public:
    /* Constructor. */
    Boulder(TileType type);

    /* Set the direction the boulder moves, if it does move. Note that
    this will change its sprite. */
    void setDirection(Map &map, const Location &place, int direction) const;

    /* Look at the map and move, bringing movables along if required. 
    Return false if it didn't move and should therefore be removed from any
    lists of boulders to try to move. */
    virtual bool update(Map &map, Location place, 
            std::vector<movable::Movable*> &movables, 
            int tick);

    /* Look at the map and see if it can move, but don't do anything. */
    virtual bool canUpdate(const Map &map, const Location &place);

    /* Figure out the direction to go from the tile sprite. */
    int getDirection(const Map &map, const Location &place) const;
};

#endif
