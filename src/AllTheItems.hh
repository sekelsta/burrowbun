#ifndef ALL_THE_ITEMS_HH
#define ALL_THE_ITEMS_HH

#include <vector>
#include "Item.hh"
#include "Tile.hh"
#include "MapHelpers.hh"

class Player;
class Map;


// All the child classes of "Item"
// Items that change the player's stats
class Potion : public Item {
    int healthGained;
    int fullnessGained;
    int manaGained;
    int woundsCured; // Removes the completely empty part of health
    int hungerCured; // Removes the completely empty part of fullness
    int manaCured; // Removed the completely empty part of mana

public:
    // Constructor
    Potion(ActionType type, std::string path);

    // What to do when used
    virtual bool use_internal(InputType type, int x, int y, World &world);
};

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
    Block(ActionType type, std::string path);

    /* Destructor must be virtual. */
    virtual ~Block();

    // What to do when used
    virtual bool use_internal(InputType type, int x, int y, World &world);
};

/* Items that can damage blocks. */
class Pickaxe: public Block {
    int blockDamage;
    int tier;
public:
    /* Constructor. */
    Pickaxe(ActionType type, std::string path);

    /* What to do when used. */
    virtual bool use_internal(InputType type, int x, int y, World &world);
};

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
    Item *makeItem(ActionType type, std::string path);
}



#endif
