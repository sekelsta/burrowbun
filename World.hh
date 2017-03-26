#ifndef WORLD_HH
#define WOLRD_HH

#include <vector>
#include <string>
#include <random> // For the fancy randomness like a normal distribution
#include "Tile.hh"
#include "MapHelpers.hh"

using namespace std;

// A class for generating a map for a sandbox game
class World {
    // Fields:

    // The array to hold the map info
    // This is a 2d array squished into 1d
    TileType *foreground;
    TileType *background;

    // The height and width of the map, in number of tiles
    int height, width;

    // Default spawn point
    Location spawn;

    // Have a random number generator
    default_random_engine generator;
    // The seed that was used to generate the world
    int seed;

    // Private methods

    // Really small helper functions that don't directly change tiles

    // Return a pointer to the tile* at x, y
    TileType *findPointer(int x, int y, TileType *array) const;

    // How to make the different types of worlds

    // Generate a world good for testing collision detection
    void generateTest();

    // Generate a tiny world good for testing world generation
    void generateSmolTest();

    // Generate an Earth-type world
    void generateEarth();

    // Parts of generating a world

    // Set all tiles to val
    void setAll(TileType val, TileType *array);

    // Start at top and go down until the type changes
    // Return the height of the first tile of a different type
    // Return -1 if there is no change
    int findChange(int x, int top, TileType *array) const;

    // Merge two arrays, adding them wherever they overlap
    vector<double> merge(int start1, int stop1, 
        const vector<double> &heights1, int start2, int stop2, 
        const vector<double> &heights2) const;

    // Set everything from y1 to y2 to tile
    void setTo(int x, int y1, int y2, TileType tile, TileType *array);

    // Set everything from y1 to y2 to something picked randomly from fill
    void setTo(int x, int y1, int y2, const vector<TileType> &fill, 
        TileType *array);

    // From x = start to x = stop, add heights[x - start] of a randomly 
    // selected Tile* from fill, above the line given by findChange(x, top)
    void addHeights(int start, int stop, const vector<double> &heights, 
        const vector<TileType> &fill, int top, TileType *array);

    // Move the line from findChange(x, top) to heights[x - start]
    void setHeights(int start, int stop, const vector<double> &heights, 
        const vector<TileType> &above, const vector<TileType> &below, int top,
        TileType *array);

    // Set all tiles in a circle with radius r centered on location center, 
    // to TileType val on the array array (eg foreground, background)
    void setCircle(float r, const Location &center, TileType val, 
        TileType *array);

    // Put a circle at each location in centers, with the corresponding radius
    // from r. r and centers should be the same length.
    void setPath(const vector<float> &r, const vector<Location> &centers,
        TileType val, TileType *array);

    // Put a gently sloping tunnel with a given start point.
    void tunnel(Location start);

    // Linear interpolator
    double lerp(double lo, double hi, double t) const;

    // Generate Perlin noise of length times * wavelength
    vector<double> noise(double range, int times, int wavelength) const;

    // Generate a heightmap recursively by midpoint displacement
    // length is best as (a power of 2) + 1, start, end, and mid are the 
    // heights at those places. variance is the coefficient of the random 
    // number added. exp is the  number that the variance is multiplied by 
    // every  iteration. For best results,
    // use a  number between 0 and 1. An exp closer to 0 will make smoother 
    // terrain, one closer to 1 will make more jagged terrain.
    vector<double> midpointDisplacement(int length, double start, double end,
        double mid, double exp, double variance);

    // Make an array containing the information for an irregular triangle with
    // width b and height h
    vector<double> makeTriangle(int b, int h, double mean, double stddev);

    /* Make a canyon, and fill it with alternating layers of fill
       This can also be used to make triangles with alternating layers of fill
       x and y are the locations of the middle of the base, width is the width
       at the base, height is distance from the peak to the base (should be
       positive for desert-style pillars or negative for canyons), fill is
       a vector of the tile objects that there can be layers of, the slope
       expected from the height and width is divided by mean to get the
       actual slope, and stddev is devietion from an exact triangle. */
    void canyon(int x, int y, int width, int height, 
        const vector<TileType> &fill, double mean, double stddev);

    // Make a mountain at x, y
    void mountain(int x, int y, int height);

    // Make an array containing the information for sinusoidal hills
    vector<double> makeHills(int length, int maxAmp, double maxFreq) const;

    // Actually put said sinusoidal hills on the map, adjusting for height
    void setHills(int start, int stop, int maxAmp, double maxFreq);

public:
    //  1-argument constructor
    World(WorldType worldType);

    // Destructor
    ~World();

    // Return the height of the map, in number of tiles
    int getHeight() const;

    // Return the width of the map, in number of tiles
    int getWidth() const;

    // Return the default spawn point
    Location getSpawn() const;

    // Returns the tile pointer at x, y
    // 0, 0 is the bottom right
    TileType getTile(int x, int y, TileType *array) const;

    // Set the tile at x, y equal to val
    void setTile(int x, int y, TileType val, TileType *array);

    // Write the map to a file
    void save(const string &filename) const;
};

#endif
