#ifndef ALL_THE_ITEMS_HH
#define ALL_THE_ITEMS_HH

#include <vector>
#include "Item.hh"
#include "Tile.hh"

// All the child classes of "Item"
// Items that change the player's stats
class Potion : public Item {
    int healthGained;
    int staminaGained;
    int manaGained;
    int woundsCured; // Removes the completely empty part of health
    int hungerCured; // Removes the completely empty part of stamina
    int manaCured; // Removed the completely empty part of mana

public:
    // Constructor
    Potion(ItemType type);

    // What to do when used
    void use(InputType type, int x, int y, Player &player, Map &map);
};

// Items that can be placed
class Block : public Item {
    TileType tileType;

public:
    // Constructor
    Block(ItemType type);

    // What to do when used
    void use(InputType type, int x, int y, Player &player, Map &map);
};

// Function to make na item of the correct class given only an item type
// Because everything deserves a namespace
namespace ItemMaker {
    /* Turn an ItemType into the corresponding TileType. Requires that the
    itemTypes and TileTypes are listed in the same order in the enum class. */
    TileType itemToTile(ItemType itemType);

    /* Turn a TileType into the corresponding ItemType. */
    ItemType tileToItem(TileType tileType);

    // Whether the type is in the vector
    bool isIn(std::vector<ItemType> items, ItemType type);

    // Take an item type and make the correct child class based on that
    Item *makeItem(ItemType type);
}



#endif
