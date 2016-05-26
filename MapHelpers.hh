#include <string>
#include <cassert>

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

// A class for keeping track of which tiles there are
enum class TileType {
    MAGMA = 200,
    EMPTY = 255,
    DIRT = 10,
    STONE = 100,
    PLATFORM = 150
};

// A class for passing a biome argument
enum class WorldType {
    TEST,
    EARTH,
    MOON
};

/* A class to make tiles based on their type, and storre their infos. */
class Tile {
public:
    // The name of this type of tile
    TileType type;

    // The height and width of the tile when displayed in game, in pixels
    int height, width;

    // The name of the image file
    std::string sprite;

    // Variables for how it interacts with the players
    bool isSolid;    // Whether players can go through it
    bool isPlatform; // Whether players collide with the underside

    // Variables to use in map generation and upkeep
    // In these ones, -1 means infinity
    float mass;        // How heavy one tile of it is
    float erodeResist;  // How hard it is to erode
    float maxTorque;   // How much it can take before it falls over
    float maxPressure; // How much pressure before it metamorphoses
    // TODO: implement heat-based metamorphism
    TileType pressureMetamorph; // What it becomes

    // Constructor, based on the tile type
    Tile(TileType tileType) {
        type = tileType;
        // Set Height and width. It's probably best if this is the same
        // for all types of tiles. Maybe it should be a field of Map.
        height = 20;
        width = 20;
        // These varables will usually be these values
        isSolid = true;
        isPlatform = false;
        erodeResist = -1;
        maxTorque = 0;
        maxPressure = -1;
        // Set things to the right values
        switch(tileType) {
            case TileType::EMPTY : 
                sprite = "empty.png";
                isSolid = false;
                mass = 0;
                break;
            case TileType::PLATFORM :
                sprite = "platform.png";
                mass = 0;
                isPlatform = true;
                break;
            case TileType::DIRT :
                sprite = "dirt.png";
                mass = 5;
                erodeResist = 10;
                maxTorque = 1;
                maxPressure = 500;
                pressureMetamorph = TileType::STONE;
                break;
            case TileType::STONE :
                sprite = "stone.png";
                mass = 10;
                erodeResist = 100;
                maxTorque = 50;
                maxPressure = 10000;
                pressureMetamorph = TileType::MAGMA;
                break;
            case TileType::MAGMA :
                sprite = "magma.png";
                mass = 15;
                maxTorque = 10;
                break;
        }
    }
};

/* A class for storing numbers that go between squares on a grid. It 
 keeps a number for the place beneath each spot and a number for the 
 place to the west of each spot. To get the number above the top row 
 or the the east of the eastmost row, ask for the thing beneath or to 
 the west of a thing with an index of numRows or numCols. */
class Grid {
private:
    int numRows, numCols;
    float *numbers;

    // Helper function for assignment operation and copy constructor
    void copyFields(const Grid &g) {
        numRows = g.getNumRows();
        numCols = g.getNumCols();
        // Copy g's array of numbers
        numbers = new float[(numRows + 1) * (numCols + 1) * 2];
        for (int i = 0; i <= numRows; i++) {
            for (int j = 0; j <= numCols; j++) {
                // Copy each value
                set(i, j, Direction::DOWN, g.get(i, j, Direction::DOWN));
                set(i, j, Direction::WEST, g.get(i, j, Direction::WEST));
            }
        }
    }
public:
    Grid(int rows, int cols) {
        numRows = rows;
        numCols = cols;
        // The + 1 is to account for above the top row and east of the east row
        numbers = new float[(numRows + 1) * (numCols + 1) * 2];
    }

    virtual ~Grid() {
        delete[] numbers;
    }

    // Copy constructor
    Grid(const Grid &g) {
        copyFields(g);
    }

    // Assignment operator
    Grid & operator=(const Grid &g) {
        delete[] numbers;
        copyFields(g);
        return *this;
    }

    int getNumRows() const {
        return numRows;
    }

    int getNumCols() const {
        return numCols;
    }

    float get(int row, int col, Direction dir) const {
        // Check that the direction is an allowed one
        assert(dir == Direction::DOWN || dir == Direction::WEST);
        assert(0 <= row);
        assert(row <= numRows);
        assert(0 <= col);
        assert(col <= numCols);

        if (dir == Direction::DOWN) {
            return numbers[2 * (row * numRows + col)];
        }
        else if (dir == Direction::WEST) {
            return numbers[2 * row * numRows + 2 * col + 1];
        }
    }

    void set(int row, int col, Direction dir, float val) {
        // Check that the direction is an allowed one
        assert(dir == Direction::DOWN || dir == Direction::WEST);
        assert(0 <= row);
        assert(row <= numRows);
        assert(0 <= col);
        assert(col <= numCols);

        if (dir == Direction::DOWN) {
            numbers[2 * (row * numRows + col)] = val;
        }
        else if (dir == Direction::WEST) {
            numbers[2 * row * numRows + 2 * col + 1] = val;
        }

    }
};

