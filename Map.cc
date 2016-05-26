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
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
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
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
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

// Returns a pointer to the tile at row, col
Tile **Map::findPointer(int row, int col) const {
    assert (0 <= row);
    assert (row < height);
    assert (0 <= col);
    assert (col < width);
    return &tiles[row * width + col];
}

Tile *Map::getTile(int row, int col) const {
    return *findPointer(row, col);
}

void Map::setTile(int row, int col, Tile *val) {
    *findPointer(row, col) = val;
}

// Set all tiles to val
void Map::setAll(Tile *val) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
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
    // temp exists because apparently Tile() only returns a temporary pointer
    Tile temp = Tile(TileType::EMPTY);
    Tile *empty = &temp;
    temp = Tile(TileType::STONE);
    Tile *stone = &temp;
    temp = Tile(TileType::PLATFORM);
    Tile *platform = &temp;
    // Set height and width, and use them to make a tile array
    height = 100;
    width = 200;
    tiles = new Tile*[height * width];
    // Set all tiles to empty, then make some that aren't
    setAll(empty);
}

// Make an Earth-style world
void Map::generateEarth() {
    // Construct generic members of relevent classes
    Tile temp = Tile(TileType::EMPTY);
    Tile *empty = &temp;
    temp = Tile(TileType::STONE);
    Tile *stone = &temp;
    temp = Tile(TileType::DIRT);
    Tile *dirt = &temp;
    temp = Tile(TileType::MAGMA);
    Tile *magma = &temp;
    temp = Tile(TileType::PLATFORM);
    Tile *platform = &temp;

    // TODO: make this actually do something interesting
    height = 500;
    width = 2000;

    // Create the array of tiles
    tiles = new Tile*[height * width];
    setAll(empty);

    // Make a horizon line for a continent
    int horizon = 200;
    int magmaLevel = 100;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < magmaLevel; y++) {
            setTile(y, x, magma);
        }
        for (int y = magmaLevel; y < horizon; y++) {
            setTile(y, x, stone);
        }
    }
    // Make a mountain TODO make this randomer
    mountain(horizon, 500);
}

// Make a mountain at row, col
void Map::mountain(int row, int col) {
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
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            outfile << 64 * (int)(getTile(i, j) -> type) << " ";
        }
        outfile << "\n";
    }
    outfile.close();
}
