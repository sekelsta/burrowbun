#ifndef ITEMMAKER_HH
#define ITEMMAKER_HH

#include "Action.hh"
#include "Item.hh"
#include "../world/Tile.hh"
#include <vector>

// Function to make an item of the correct class given only an item type
// Because everything deserves a namespace
namespace ItemMaker {
    /* Turn an ActionType into the corresponding TileType. Requires that the
    ActionTypes and TileTypes are listed in the same order in the 
    enum class. */
    TileType itemToTile(ActionType ActionType);

    /* Turn a TileType into the corresponding ActionType. */
    ActionType tileToItem(TileType tileType);

    // Whether the type is in the vector
    bool isIn(std::vector<ActionType> items, ActionType type);

    // Take an item type and make the correct child class based on that
    Item *makeItem(ActionType type);
}



#endif
