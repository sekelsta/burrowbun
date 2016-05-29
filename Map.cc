#include <iostream>
#include "Map.hh"

using namespace std;

// Constructors, destructor, and assignment operation
// Constructor, based on the world type given
Map::Map(WorldType worldType) {
    // Run the appropriate function
    if (worldType == WorldType::EARTH) {
        generateEarth();
    }
    else if (worldType == WorldType::TEST) {
        generateTest();
    }
    else {
        cerr << "Maybe I'll implement that later." << endl;
    }
}

// Copy-constructor
Map::Map(const Map &m) {
    copyFields(m);
}

// Destructor
Map::~Map() {
    delete[] tiles;
}

// Helper function for copy-constructor and assignment operator
void Map::copyFields(const Map &m) {
    // If I ever add more fields, I'll need to change this
    height = m.getHeight();
    width = m.getWidth();
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            setTile(i, j, m.getTile(i, j));
        }
    }
}

// Set equal
Map & Map::operator=(const Map &m) {
    if (this != &m) {
        delete[] tiles;
        copyFields(m);
    }
    return *this;
}

// Return true if this map is the same as the specified map
bool Map::operator==(const Map &map) const {
    // If I ever add more fields, I'll need to change this
    if (height != map.getHeight()) { return false; }
    if (width != map.getWidth()) { return false; }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (getTile(i, j) != map.getTile(i, j)) { return false; }
        }
    }

    // If it hasn't returned false by now, all the fields must be the same
    return true;
}

// Return false if this map is the same as the specified map
bool Map::operator!=(const Map &map) const {
    return !(*this == map);
}

// Access fields
int Map::getHeight() const {
    return height;
}

int Map::getWidth() const {
    return width;
}

// Returns a pointer to the tile at x, y
// I should probably just make tiles a 2d array
Tile **Map::findPointer(int x, int y) const {
    assert (0 <= x);
    assert (x < width);
    assert (0 <= y);
    assert (y < height);
    return &tiles[y * width + x];
}

Tile *Map::getTile(int x, int y) const {
    return *findPointer(x, y);
}

void Map::setTile(int x, int y, Tile *val) {
    *findPointer(x, y) = val;
}

// Set all tiles to val
void Map::setAll(Tile *val) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            setTile(i, j, val);
        }
    }
}

// Make a world good for testing collision detection
void Map::generateTest() {
    // Construct generic members of relevent classes
    // I'm not convinced that this is the best way to implement it, but
    // it allows the storage of more information than just having an 
    // array of TileType, and is less bulky than construcing a new member
    // of the class for each one.
    // Later types of tiles might need to be unique objects
    // Except this doesn't work because the variables go out of scope
    Tile empty = Tile(TileType::EMPTY);
    Tile stone = Tile(TileType::STONE);
    Tile platform = Tile(TileType::PLATFORM);
    // Set height and width, and use them to make a tile array
    height = 100;
    width = 200;
    tiles = new Tile*[height * width];
    // Set all tiles to empty, then make some that aren't
    setAll(&empty);

    // Make the bottom solid
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < 20; j++) {
            setTile(i, j, &stone);
        }
    }

    // Add a couple lines and some platforms
    for (int i = 20; i < 80; i++) {
        setTile(i, 40, &stone);
        setTile(i, 50, &platform);
        setTile(i, 60, &stone);
    }

    // Add some diagonal lines
    for (int i = 20; i < 60; i++) {
        setTile(100 + i, i + 10, &stone);
        setTile(100 - i, i + 10, &stone);
    }
}

// Make an Earth-style world
void Map::generateEarth() {
    // Construct generic members of relevent classes, since we don't
    // actually need a new Tile object for every instance of that type
    // Except this doesn't work because the variables go out of scope
    Tile empty = Tile(TileType::EMPTY);
    Tile stone = Tile(TileType::STONE);
    Tile dirt = Tile(TileType::DIRT);
    Tile magma = Tile(TileType::MAGMA);
    Tile platform = Tile(TileType::PLATFORM);

    // TODO: make this actually do something interesting
    height = 10;
    width = 5;

    // Create the array of tiles
    tiles = new Tile*[height * width];
    setAll(&empty);

    // Make a horizon line for a continent
    int horizon = 4;
    int magmaLevel = 2;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < magmaLevel; y++) {
            setTile(x, y, &magma);
            assert(getTile(x, y) -> type == TileType::MAGMA);
        }
        for (int y = magmaLevel; y < horizon; y++) {
            setTile(x, y, &stone);
            assert(getTile(x, y) -> type == TileType::STONE);
        }
    }
    // Make a mountain TODO make this randomer
    mountain(horizon, 500);
}

// Make a mountain at x, y
void Map::mountain(int x, int y) {
//TODO
}

// Write the map to a file
void Map::save(string filename) const {
    // Saves in .bmp file format in black and white
    ofstream outfile;
    outfile.open(filename);

    // Write an informative header
    outfile << "P2\n# Map\n" << width << " " << height << "\n255\n";
    // Write tile values
    for (int j = height - 1; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
            outfile << (int)(getTile(i, j) -> type) << " ";
        }
        outfile << "\n";
    }
    outfile.close();
}
