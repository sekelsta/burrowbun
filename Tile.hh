#ifndef TILE_HH
#define TILE_HH

#include <vector>
#include <string>
#include "Sprite.hh"
#include "Movable.hh"
#include "Light.hh"
#include "Damage.hh"

/* Forward declare! */
class Map;
struct Location;

// A class for keeping track of which tiles there are
enum class TileType : short {
    EMPTY,
    WATER,
    DIRT,
    TOPSOIL,
    CLAY,
    CALCAREOUS_OOZE,
    SNOW,
    ICE,
    STONE,
    GRANITE,
    BASALT,
    LIMESTONE,
    MUDSTONE,
    PERIDOTITE,
    SANDSTONE,
    RED_SANDSTONE,
    PLATFORM,
    LUMBER,
    RED_BRICK,
    GRAY_BRICK,
    DARK_BRICK,
    SAND,
    MUD,
    CLOUD,
    BOULDER,
    GLACIER,
    FIRST_TILE = EMPTY,
    FIRST_ITEMED_TILE = DIRT, // First tile with an equivalent item
    LAST_TILE = GLACIER,
    LAST_PURE_TILE = DARK_BRICK, // Last tile that's not a subclass
    FIRST_BOULDER = SAND,
    LAST_BOULDER = GLACIER
};

/* A class to make tiles based on their type, and store their infos. */
// Maybe since maps are filled with pointers to the same tile, everything
// should be constant?
class Tile {
    // Collision-related varables
    /* Whether the tile is a platform. Palyers only colide with the tops side
    of platforms. */
    bool isPlatform;
    /* Whether players can pass through the tile. */
    bool isSolid;
    /* How much damage the player takes from sharing space with this tile. */
    Damage overlapDamage;

    // Display-related variables
    /* Tiles with 0 opacity are completely permeable to light. */
    int opacity;

    // Mining-related variables
    /* How much health the tile has determines how many hits it can take from
    a pickax before it breaks. */
    int maxHealth;

protected:
    /* Return the filename of the json file for that tiletype. */
    static std::string getFilename(TileType tileType);

public:
    // The name of this type of tile
    const TileType type;

    // Information about the sprite
    // Tile spritesheets use rows for the different versions that depend on
    // whether each side is next to air, and the cols are the different 
    // variations.
    Sprite sprite;

    /* What color should represent it in images. */
    Light color;

    // Variables for how it interacts with the players
    bool getIsPlatform() const;
    bool getIsSolid() const;
    void dealOverlapDamage(movable::Movable &movable) const;

    /* For lighting. Tiles with 0 opacity are completely permeable to light.*/
    int getOpacity() const;

    // Basically the number of hits with a pickax to break it
    int getMaxHealth() const;

    /* Which sprite on the spritesheet to use. */
    virtual uint8_t getSpritePlace(const Map &map, const Location &place)
             const;

    /* What sprite to change to. */
    virtual uint8_t updateSprite(const Map &map, const Location &place)
            const;

    /* Change the map in whatever way needs doing. */
    virtual bool update(Map &map, Location place, 
        std::vector<movable::Movable*> &movables, int tick);

    // Constructor, based on the tile type
    Tile(TileType tileType);

    /* Virtual destructor. */
    virtual ~Tile();

    /* Whether the tile will ever need to call its update function. */
    virtual bool canUpdate(const Map &map, const Location &place);

};

#endif
