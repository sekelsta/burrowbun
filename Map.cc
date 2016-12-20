#include <iostream>
#include <fstream> // To read and write files
#include <cassert>
#include <ctime> // To seed the random number generator
#include <cstdlib> // For randomness
#include <math.h> // Because pi
#include "Map.hh"

using namespace std;

// Private methods

// Really small helper functions that don't directly change tiles

// Returns a pointer to the SpaceInfo at x, y
SpaceInfo *Map::findPointer(int x, int y) const {
    assert (0 <= x);
    assert (x < width);
    assert (0 <= y);
    assert (y < height);
    return tiles + (y * width + x);
}

// Make a new Tile *, add it to the list of pointers, and return the pointer
Tile *Map::newTile(TileType val) {
    Tile *tile = new Tile(val, pointers.size());
    pointers.push_back(tile);
    return tile;
}

// Find a Tile object of type val. If it does not exist, create it. If 
// multiple exist, return the first one found.
Tile *Map::makeTile(TileType val) {
    for (unsigned int i = 0; i < pointers.size(); i++) {
        if (pointers[i] -> type == val) {
            return pointers[i];
        }
    }
    return newTile(val);
}

/*
/ / Set all tiles to val
void Map::setAll(Tile* const &val) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            setTile(i, j, val);
        }
    }
}
*/
// Public methods

// Constructor, based on a world file that exists
Map::Map(string filename) {
    ifstream infile(filename);

    // Check that the file could be opened
    // TODO: use exceptions like a proper person
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }

    // Check that the header is #Map
    string header;
    infile >> header;
    if (header != "#Map") {
        cerr << filename << " doesn't say it's a map." << "\n";
    }

    // Read the height and width
    infile >> width >> height;

    // Create an array of tiles
    tiles = new SpaceInfo[width * height];

    // Read the spawn point
    infile >> spawn.x >> spawn.y;

    // Read the map
    int index = 0;
    int count, tile;
    TileType current;
    Tile *matchingTile = NULL;
    while (index < height * width) {
        infile >> count >> tile;
        current = (TileType)tile;
        matchingTile = makeTile(current);
        for (int i = 0; i < count; i++) {
            assert(index < width * height);
            tiles[index].foreground = matchingTile;
            tiles[index].foregroundHealth = matchingTile -> maxHealth;
            tiles[index].background = NULL;
            tiles[index].backgroundHealth = 1;
            index++;
        }
    }
    assert(getForeground(0, 0) -> type == tiles[0].foreground -> type);
}

// Destructor
Map::~Map() {
    // Delete the map
    delete[] tiles;
    // Delete each tile object
    while (pointers.empty() == false) {
        delete pointers.back();
        pointers.pop_back();
    }
}


// Return the height of the map, in number of tiles
int Map::getHeight() const {
    return height;
}

// Return the width of the map, in number of tiles
int Map::getWidth() const {
    return width;
}

// Return the default spawn point
Location Map::getSpawn() const {
    return spawn;
}

// Returns the foreground tile at x, y
// 0, 0 is the bottom right
Tile *Map::getForeground(int x, int y) const {
    return findPointer(x, y) -> foreground;
}

// Returns the background tile at x, y
// 0, 0 is the bottom right
Tile *Map::getBackground(int x, int y) const {
    return findPointer(x, y) -> background;
}

// Set the foreground tile at x, y equal to val
void Map::setForeground(int x, int y, Tile* const &val) {
    findPointer(x, y) -> foreground = val;
    findPointer(x, y) -> foregroundHealth = val -> maxHealth;
}

// Set the background tile at x, y equal to val
void Map::setBackground(int x, int y, Tile* const &val) {
    findPointer(x, y) -> background = val;
    findPointer(x, y) -> backgroundHealth = val -> maxHealth;
}

// Gets the map's list of the tile pointers it uses
vector<Tile *> Map::getPointers() const {
    return pointers;
}

// Gets a reference to the list of pointers the map uses
vector<Tile *> &Map::getPointersRef() {
    return pointers;
}

// Write the map to a file
void Map::save(const string &filename) const {
    // Saves in .bmp file format in black and white
    ofstream outfile;
    outfile.open(filename);

    // Write an informative header
    outfile << "P2\n# Map\n" << width << " " << height << "\n255\n";
    // Write tile values
    for (int j = height - 1; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
            outfile << (int)(getForeground(i, j) -> type) << " ";
        }
        outfile << "\n";
    }
    outfile.close();
}

