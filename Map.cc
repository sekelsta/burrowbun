#include <iostream>
#include <fstream> // To read and write files
#include <cassert>
#include <ctime> // To seed the random number generator
#include <cstdlib> // For randomness
#include <math.h> // Because pi
#include <tgmath.h> // for exponentiation
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

/* Pick the sprite for a tile based on the ones next to it. */
void Map::chooseSprite(int x, int y) {
    // The value at getSpritePlace.y should be between 0 and 15.
    // In fact, it should be the binary number that you get if
    // you start at the top and go counterclockwise, treating
    // a non-air tile to that side as 0 and an air tile to that 
    // side as 1.
    int col = 0;
    if (y != height - 1 
        && getForeground(x, y + 1) -> type == TileType::EMPTY) {
            col += 1;
    }
    if (x != width - 1
        && getForeground(x + 1, y) -> type == TileType::EMPTY) {
            col += 2;
    }
    if (y != 0
        && getForeground(x, y - 1) -> type == TileType::EMPTY) {
            col += 4;
    }
    if (x != 0
        && getForeground(x - 1, y) -> type == TileType::EMPTY) {
            col += 8;
    }
    findPointer(x, y) -> spritePlace.y = col;
    int row = rand() % getForeground(x, y) -> sprite.cols;
    findPointer(x, y) -> spritePlace.x = row;
}

/* Return true if neither the foreground nor background is opaque. */
inline bool Map::isSky(int x, int y) {
    return (getForeground(x, y) -> opacity == 0
        && getBackground(x, y) -> opacity == 0);
}

/* Return the square of the distance between i, j and x, y. */
int Map::distance(int i, int j, int x, int y) {
    return (((i - x) * (i - x)) + ((j - y) * (j - y)));
}

/* Compute the square of the distance to the nearest sky tile that is a source 
   of light (doesn't have an opaque foreground or background). If the distance 
   is more than maxDist, return maxDist. */
int Map::skyDistance(int x, int y, int maxDist) {
    if (isSky(x, y)) {
        return 0;
    }
    int smallest = distance(0, 0, 0, maxDist);
    for (int i = max(0, x + 1 - maxDist); i < maxDist + x && i < width; 
            i++) {
        for (int j = max(0, y + 1 - maxDist);
                j < maxDist + y && j < height; j++) {
            if (isSky(i, j)) {
                smallest = min(smallest, distance(i, j, x, y));
            }
        }
    }
    return smallest;
}

/* Calculate how well-lit the tile is. */
void Map::setLight(int x, int y) {
    // TODO: actually make this depend on the light source
    SpaceInfo *place = findPointer(x, y);
    place -> lightBlock.r = 0;
    place -> lightBlock.g = 0;
    place -> lightBlock.b = 0;
    int dist = skyDistance(x, y, 25);
    place -> lightSky.r = 255;
    place -> lightSky.g = 255;
    place -> lightSky.b = 255;
    place -> lightSky.setIntensity(max(0.0, exp((1 - dist) / 8.0)));
}

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
    Tile *matchingBackground = NULL;
    while (index < height * width) {
        infile >> count >> tile;
        current = (TileType)tile;
        matchingTile = makeTile(current);
        matchingBackground = makeTile(TileType::EMPTY);
        for (int i = 0; i < count; i++) {
            assert(index < width * height);
            tiles[index].foreground = matchingTile;
            tiles[index].foregroundHealth = matchingTile -> maxHealth;
            tiles[index].background = matchingBackground;
            tiles[index].backgroundHealth = 1;
            tiles[index].spritePlace.x = rand() % matchingTile -> sprite.cols;
            tiles[index].spritePlace.y = 0;
            index++;
        }
    }
    assert(getForeground(0, 0) -> type == tiles[0].foreground -> type);

    /* Iterate over the map finding the tiles that need a special border 
       sprite and figuring out how well-lit each tile is. */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (getForeground(i, j) -> type != TileType::EMPTY) {
                chooseSprite(i, j);
            }
            setLight(i, j);
        }
    }
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

// Return the part of the spritesheet that should be used
Location Map::getSpritePlace(int x, int y) const {
    return findPointer(x, y) -> spritePlace;
}

// Return lighting from torches and other blocks
Light Map::getSkyLight(int x, int y) const {
    return findPointer(x, y) -> lightSky;
}

// Return lighting from proximity to the sky
Light Map::getBlockLight(int x, int y) const {
    return findPointer(x, y) -> lightBlock;
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

