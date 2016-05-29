#include <iostream>
#include <fstream> // To write and read files
#include <cassert>
#include <cstdlib> // For randomness
#include <ctime> // To seed the random number generator
#include "Tile.hh"
#include "MapHelpers.hh"

using namespace std;


// A class for a map for a sandbox game
// Currently its only focus is on generating the map
class Map {
    // The array to hold the map info
    // This is a 2d array squished into 1d
    Tile **tiles;

    // The height and width of the map, in number of tiles
    int height, width;

    // Return a pointer to the tile* at x, y
    Tile **findPointer(int x, int y) const;

    // Copy fields, to make assignment and copy-construction eaasier
    void copyFields(const Map &m);

    // Set all tiles to val
    void setAll(Tile *val);

    // Generate a world good for testing collision detection
    void generateTest();

    // Generate an Earth-type world
    void generateEarth();

    // Make a mountain at x, y
    void mountain(int x, int y);

public:
    //  1-argument constructor
    Map(WorldType worldType);

    // Copy-constructor
    Map(const Map &m);

    // Destructor
    ~Map();

    // Map assignment operator
    Map & operator=(const Map &m);

    // Returns true if this map is the same as the specified map
    bool operator==(const Map &map) const;

    // Returns false if this map is the same as the specified map
    bool operator!=(const Map &map) const;

    // Return the height of the map, in number of tiles
    int getHeight() const;

    // Return the width of the map, in number of tiles
    int getWidth() const;

    // Return the value of the tile at x, y
    // 0, 0 is the bottom right
    Tile *getTile(int x, int y) const;

    // Set the tile at x, y equal to val
    void setTile(int x, int y, Tile *val);

    // Write the map to a file
    void save(string filename) const;
};
