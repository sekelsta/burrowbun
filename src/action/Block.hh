#ifndef BLOCK_HH
#define BLOCK_HH

#include "Item.hh"
#include "../entity/Player.hh"
#include "../world/Map.hh"
#include "../world/MapHelpers.hh"
#include "../world/Tile.hh"
#include "../world/World.hh"

// Items that can be placed
class Block : public Item {
    /* The type of the associated tile. */
    TileType tileType;
protected:
    /* For blocks that let the player place them extra far away. */
    int bonusReach;

    /* Tell whether the player can reach far enough to place a block here. */
    bool canPlace(int x, int y, const Player &player, const Map &map);

    /* If it was a left click, return foreground, if right click, return
    background. */
    MapLayer getLayer(InputType type);

public:
    // Constructor
    Block(ActionType type);

    /* Destructor must be virtual. */
    virtual ~Block();

    // What to do when used
    virtual bool use_internal(InputType type, int x, int y, World &world);
};

#endif
