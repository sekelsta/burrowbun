#include <vector>
#include <string>
#include <iostream>
#include <fstream> // To write and read files
#include <cassert>
#include <cstdlib> // For randomness
#include <ctime> // To seed the random number generator
#include <random> // For the fancy randomness like a normal distribution
#include <math.h> // because pi
#include "Tile.hh"

using namespace std;

// A simple class for representing locations in a 2D array.  The class also
// implements equality/inequality operators so that we can see if two
// locations are the same or not.
class Location {
public:
    // The row and column of the location
    int row;
    int col;

    // Constructors for initializing locations
    Location(int row, int col) : row(row), col(col) { }
    Location() : row(0), col(0) { }
    
    // Returns true if this location is the same as the specified location
    bool operator==(const Location &loc) const {
        return row == loc.row && col == loc.col;
    }
    
    // Returns true if this location is different from the specified location
    bool operator!=(const Location &loc) const {
        return !(*this == loc);
    }
};

// A class to talk about directions. Down and West are smaller numbers
enum class Direction {
    DOWN,
    UP,
    WEST,
    EAST
};

/* A class for passing a biome argument, to pick which pattern of 
world-generation to use. */
enum class WorldType {
    TEST,
    EARTH,
    MOON
};

// A class for a map for a sandbox game
// Currently its only focus is on generating the map
class Map {
    // Fields:

    // The array to hold the map info
    // This is a 2d array squished into 1d
    const Tile **tiles;

    // A list of the pointers in the map
    // Basically these are the ones that contain manually allocated memeory
    vector<const Tile *> pointers;

    // The height and width of the map, in number of tiles
    int height, width;

    // Default spawn point
    Location spawn;

    // Have a random number generator
    default_random_engine generator;

    // Private methods

    // Really small helper functions that don't directly change tiles

    // Return a pointer to the tile* at x, y
    const Tile **findPointer(int x, int y) const;

    /* Make a Tile object, add it to the list of pointers, and return 
       a pointer to it. */
    const Tile *makeTile(TileType val);

    // How to make the different types of worlds

    // Generate a world good for testing collision detection
    void generateTest();

    // Generate an Earth-type world
    void generateEarth();

    // Parts of generating a world

    // Set all tiles to val
    void setAll(const Tile* &val);

    // Start at top and go down until the type changes
    // Return the height of the first tile of a different type
    // Return -1 if there is no change
    int findChange(int x, int top) const;

    // Merge two arrays, adding them wherever they overlap
    vector<double> merge(int start1, int stop1, 
                    const vector<double> &heights1, int start2, 
                    int stop2, const vector<double> &heights2) const;

    // Set everything from y1 to y2 to tile
    void setTo(int x, int y1, int y2, const Tile* &tile);

    // Set everything from y1 to y2 to something picked randomly from fill
    void setTo(int x, int y1, int y2, const vector<const Tile *> &fill);

    // From x = start to x = stop, add heights[x - start] of a randomly 
    // selected Tile* from fill, above the line given by findChange(x, top)
    void addHeights(int start, int stop, const vector<double> &heights, 
                    const vector<const Tile *> &fill, int top);

    // Move the line from findChange(x, top) to heights[x - start]
    void setHeights(int start, int stop, const vector<double> &heights, 
                    const vector<const Tile *> &above, 
                    const vector<const Tile *> &below, int top);

    // Linear interpolator
    double lerp(double lo, double hi, double t) const;

    // Generate Perlin noise of length times * wavelength
    vector<double>noise(double range, int times, int wavelength) const;

    // Make an array containing the information for an irregular triangle with
    // width b and height h
    vector<double>makeTriangle(int b, int h, double mean, double stddev);

    /* Make a canyon, and fill it with alternating layers of fill
       This can also be used to make triangles with alternating layers of fill
       x and y are the locations of the middle of the base, width is the width
       at the base, height is distance from the peak to the base (should be
       positive for desert-style pillars or negative for canyons), fill is
       a vector of the tile objects that there can be layers of, the slope
       expected from the height and width is divided by mean to get the
       actual slope, and stddev is devietion from an exact triangle. */
    void canyon(int x, int y, int width, int height, 
                    const vector<const Tile *> &fill, 
                    double mean, double stddev);

    // Make a mountain at x, y
    void mountain(int x, int y, int height);

    // Make an array containing the information for sinusoidal hills
    vector<double>makeHills(int length, int maxAmp, double maxFreq) const;

    // Actually put said sinusoidal hills on the map, adjusting for height
    void setHills(int start, int stop, int maxAmp, double maxFreq);

public:
    //  1-argument constructor
    Map(WorldType worldType);

    // Destructor
    ~Map();

    // Return the height of the map, in number of tiles
    int getHeight() const;

    // Return the width of the map, in number of tiles
    int getWidth() const;

    // Return the default spawn point
    Location getSpawn() const;

    // Returns the tile pointer at x, y
    // 0, 0 is the bottom right
    const Tile *getTile(int x, int y) const;

    // Set the tile at x, y equal to val
    void setTile(int x, int y, const Tile* const &val);

    // Write the map to a file
    void save(const string &filename) const;
};
