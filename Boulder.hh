#ifndef BOULDER_HH
#define BOULDER_HH

#include <vector>
#include "Tile.hh"

/* A Tile that moves. */
class Boulder: public Tile {
    /* How many ticks to wait before moving left or right. If this is 0,
    then this type of boulder never moves left of right. */
    int moveTicks;

    /* List of tile types which drop as an item when this type of boulder
    runs into them sideways. */
    vector<TileType> tilesDestroyed;

    /* Same, but for falling. */
    vector<TileType> tilesCrushed;

    /* List of tile types which this boulder will switch places with. */
    vector<TileType> tilesDisplaced;

    /* Same, but for falling. */
    vector<TileType> tilesSunk;

    /* Does it float or fall? */
    bool isFloating;

    /* If someone stands on it while it moves, does it bring them? */
    bool carriesMovables;

    /* Convert a vector<int> to a vector<TileType> */
    static vector<TileType> vectorConvert(const vector<int> &input);

public:
    /* Constructor. */
    Boulder(TileType type);

    /* Look at the map and move, bringing movables along if required. 
    Return false if it didn't move and should therefore be removed from any
    lists of boulders to try to move. */
    bool update(Map &map, Location place, vector<Movable*> &movables, 
        int tick);

};

#endif
