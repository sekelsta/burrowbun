#include <iostream>
#include <fstream>
#include <cassert>
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

    // Return a pointer to the tile* at row, col
    Tile **findPointer(int row, int col) const;

    // Copy fields, to make assignment and copy-construction eaasier
    void copyFields(const Map &m);

    // Set all tiles to val
    void setAll(Tile *val);

    // Generate a world good for testing collision detection
    void generateTest();

    // Generate an Earth-type world
    void generateEarth();

    // Make a mountain at row, col
    void mountain(int row, int col);

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

    // Return the value of the tile at row, col
    // 0, 0 is the bottom right
    Tile *getTile(int row, int col) const;

    // Set the tile at row, col equal to val
    void setTile(int row, int col, Tile *val);

    // Write the map to a file
    void save(string filename) const;
};
