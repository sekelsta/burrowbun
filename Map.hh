#ifndef MAP_HH
#define MAP_HH

#include <vector>
#include <set>
#include <string>
#include <random> // For the fancy randomness like a normal distribution
#include "Tile.hh"
#include "MapHelpers.hh"

// A class for a map for a sandbox game
class Map {
    // Fields:

    /* How many ticks since the map was loaded. */
    unsigned int tick;

    // The array to hold the map info
    // This is a 2d array squished into 1d
    SpaceInfo *tiles;

    /* A list of Tiles. They contain memory that must be manually garbage
    collected becuase of the SDL textures. */
    std::vector<Tile *> pointers;

    // The height and width of the map, in number of tiles
    int height, width;

    // The height and width of the tiles, in pixels
    const int TILE_WIDTH;
    const int TILE_HEIGHT;

    // Default spawn point
    Location spawn;

    /* The tiles whose update function should be called. */
    std::set<Location> toUpdate;

    /* Tiles that have been damaged. */
    std::vector<TileHealth> damaged;

    // Have a random number generator
    std::default_random_engine generator;
    // The seed that was used to generate the map
    int seed;

    // Private methods

    // Really small helper functions that don't directly change tiles

    // Return a pointer to the SpaceInfo* at x, y
    SpaceInfo *findPointer(int x, int y) const;

    /* Make a Tile object, add it to the list of pointers, and return 
       a pointer to it. */
    Tile *newTile(TileType val);

    // Find a Tile object of type val. If it does not exist, create it. If
    // multiple exist, return the first one.
    Tile *getTile(TileType val);

    /* Pick the sprite to use for a tile based on the ones next to it. */
    void chooseSprite(int x, int y);

    /* Return true if the spot is empty of foreground tiles but at least 
    one tile next to it isn't. */
    bool isBesideTile(int x, int y, MapLayer layer);

    /* Return true if neither the foreground nor background are opaque. */
    bool isSky(int x, int y);

    /* Return the square of the distance between i, j and x, y. */
    int distance(int i, int j, int x, int y);

    /* Compute the taxicab distance to the nearest sky block that is a source 
       of light (doesn't have and opaque foreground or background). If more 
       than maxDist, return maxDist. */
    int skyDistance(int x, int y, int maxDist);

    /* Set the light of a tile. */
    void setLight(int x, int y);

    /* Set the tiles around a place to show the right sprite and have the
    right amount of light. */
    void updateNear(int x, int y);

    public:

    /* Add a place to the list of places to be updated, if the tile there
    will need to be updated. */
    void addToUpdate(int x, int y, MapLayer layer);
    void addToUpdate(const Location &place);

    void removeFromUpdate(int x, int y, MapLayer layer);
    void removeFromUpdate(const Location &place);

    bool updateContains(const Location &place) const;

    /* Return a number from 0-15 depending on which tiles border this one. */
    int bordering(const Location &place) const;

    /* Return true if this is a place that exists on the map. */
    bool isOnMap(int x, int y) const;

    void loadLayer(MapLayer layer, std::ifstream &infile);

    // Constructor, constructs a map by loading a file
    Map(std::string filename, int tileWidth, int tileHeight);

    // Destructor
    ~Map();

    // Return the height of the map, in number of tiles
    int getHeight() const;

    // Return the width of the map, in number of tiles
    int getWidth() const;

    // Return the height, in pixels of each tile
    int getTileHeight() const;

    // Retrun the width, in pixels, of each tile
    int getTileWidth() const;

    // Return the default spawn point
    Location getSpawn() const;

    // Return which part of the spritesheet should be used
    uint8_t getForegroundSprite(int x, int y) const;
    uint8_t getBackgroundSprite(int x, int y) const;

    // Return the light at a square, setting it if necessary. 
    Light getLight(int x, int y);

    // Return the color the sun / moon is shining
    Light getSkyColor(int x, int y) const;

    /* Return the pointer the the tile at this location. */
    Tile *getTile(Location place);
    Tile *getTile(int x, int y, MapLayer layer);

    // Returns the foreground tile pointer at x, y
    // 0, 0 is the bottom right
    Tile *getForeground(int x, int y);

    // Returns the background tile pointer at x, y
    // 0, 0 is the bottom right
    Tile *getBackground(int x, int y);

    /* Get the type of the tile at place.x + x, place.y + y, place.layer. */
    TileType getTileType(const Location &place, int x, int y) const;

    /* Set the tile at x, y, layer equal to val. */
    void setTile(int x, int y, MapLayer layer, TileType val);
    void setTile(const Location &place, TileType val);

    /* Place a tile in the correct layer. Return whether it was successful. */
    bool placeTile(Location place, TileType type);

    // Gets the map's list of the tile pointers it uses
    std::vector<Tile *> getPointers() const;

    // Gets a reference to the map's list of the tile pointers it uses
    // This should only be used to set the tile textures
    std::vector<Tile *> &getPointersRef();

    // Write the map to a file
    void save(const std::string &filename);

    /* Update the map. */
    void update(std::vector<movable::Movable*> &movables);

    /* Damage a tile (with a pickax or something). Return false if there
    was no tile to damage. */
    bool damage(Location place, int amount);

    /* Destroy a tile if it has no health. Return true if it was destroyed, or
    false if it still had health and lived. */
    bool destroy(const TileHealth &health);

    /* Destroy a tile. */
    void kill(int x, int y, MapLayer layer);
    void kill(const Location &place);

    /* Take an invalid x location and add or subtract width until
    0 <= x < width. */
    int wrapX(int x) const;

    /* Take in world coordinates and a layer and convert to a location in 
    map coordinates. */
    Location getMapCoords(int x, int y, MapLayer layer);

    /* Move a tile one down. If it was just above the bottom of the map, 
    it ceases to exist. It there's a tile in the way, it will be destroyed. */
    void moveDown(const Location &place);

    /* Switch this tile with the one below it. */
    void displaceDown(const Location &place);

    /* Move a tile dist in the +x direction. If there's a tile in the way,
    it will be destroyed. */
    void moveSideways(const Location &place, int dist);

    /* Move a tile dist in the +x direction. If there's a tile there, they
    switch places. */
    void displaceSideways(const Location &place, int dist);
};

#endif
