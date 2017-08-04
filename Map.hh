#ifndef MAP_HH
#define MAP_HH

#include <vector>
#include <string>
#include <random> // For the fancy randomness like a normal distribution
#include "Tile.hh"
#include "MapHelpers.hh"

using namespace std;

// A class for a map for a sandbox game
// Currently its only focus is on generating the map
class Map {
    // Fields:

    /* How many ticks since the map was loaded. */
    unsigned int tick;

    // The array to hold the map info
    // This is a 2d array squished into 1d
    SpaceInfo *tiles;

    // A list of the pointers in the map
    // Basically these are the ones that contain manually allocated memeory
    vector<Tile *> pointers;

    // The height and width of the map, in number of tiles
    int height, width;

    // The height and width of the tiles, in pixels
    const int TILE_WIDTH;
    const int TILE_HEIGHT;

    // Default spawn point
    Location spawn;

    /* The tiles whose update function should be called. */
    vector<Location> toUpdate;

    /* Tiles that have been damaged. */
    vector<TileHealth> damaged;

    // Have a random number generator
    default_random_engine generator;
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
    bool isBesideTile(int x, int y, MapLayer layer) const;

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

    /* Return true if this is a place that exists on the map. */
    bool isOnMap(int x, int y) const;

    public:

    // Constructor, constructs a map by loading a file
    Map(string filename, int tileWidth, int tileHeight);

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
    Location getSpritePlace(int x, int y) const;

    // Return the light at a square, setting it if necessary. 
    Light getLight(int x, int y);

    // Return the color the sun / moon is shining
    Light getSkyColor(int x, int y) const;

    /* Return the pointer the the tile at this location. */
    Tile *getTile(Location place) const;
    Tile *getTile(int x, int y, MapLayer layer) const;

    // Returns the foreground tile pointer at x, y
    // 0, 0 is the bottom right
    Tile *getForeground(int x, int y) const;

    // Returns the background tile pointer at x, y
    // 0, 0 is the bottom right
    Tile *getBackground(int x, int y) const;

    /* Set the tile at x, y, layer equal to val. */
    void setTile(int x, int y, MapLayer layer, Tile* const &val);
    void setTile(const Location &place, Tile* const &val);

    /* Place a tile in the correct layer. Return whether it was successful. */
    bool placeTile(int x, int y, TileType type, MapLayer layer);

    // Gets the map's list of the tile pointers it uses
    vector<Tile *> getPointers() const;

    // Gets a reference to the map's list of the tile pointers it uses
    // This might only be for setting the tile textures
    vector<Tile *> &getPointersRef();

    // Write the map to a file
    void save(const string &filename) const;

    /* Update the map. */
    void update(vector<Movable*> &movables);

    /* Damage a tile (with a pickax or something). Return false if there
    was no tile to damage. */
    bool damage(int x, int y, int amount, MapLayer layer);

    /* Destroy a tile if it has no health. Return true if it was destroyed, or
    false if it still had health and lived. */
    bool destroy(const TileHealth &health);
};

#endif
