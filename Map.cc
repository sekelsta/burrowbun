#include "Map.hh"

using namespace std;

// Private methods

// Really small helper functions that don't directly change tiles

// Returns a pointer to the tile* at x, y
// Maybe I should just make tiles a 2d array
Tile **Map::findPointer(int x, int y) const {
    assert (0 <= x);
    assert (x < width);
    assert (0 <= y);
    assert (y < height);
    return &tiles[y * width + x];
}

// Make a new Tile *, add it to the list of pointers, and return the pointer
Tile *Map::makeTile(TileType val) {
    Tile *tile = new Tile(val);
    pointers.push_back(tile);
    return tile;
}

// How to make the different types of worlds

// Make a world good for testing collision detection
void Map::generateTest() {
    // Construct generic members of relevent classes
    // I'm not convinced that this is the best way to implement it, but
    // it allows the storage of more information than just having an 
    // array of TileType, and is less bulky than construcing a new member
    // of the class for each one.
    // Later types of tiles might need to be unique objects
    // Also adds the addresses of the tile to the vector pointers
    // TODO: bugfix so it actually makes the shape it should
    Tile *empty = makeTile(TileType::EMPTY);
    Tile *stone = makeTile(TileType::STONE);
    Tile *platform = makeTile(TileType::PLATFORM);
    // Set height and width, and use them to make a tile array
    height = 500;
    width = 500;
    tiles = new Tile*[height * width];
    // Set all tiles to empty, then make some that aren't
    setAll(empty);

    // Make the bottom solid
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < 20; j++) {
            setTile(i, j, stone);
        }
    }

    // Add a couple lines and some platforms
    for (int i = 20; i < 80; i++) {
        setTile(i, 40, stone);
        setTile(i, 50, platform);
        setTile(i, 60, stone);
    }

    // Add some diagonal lines
    for (int i = 20; i < 60; i++) {
        setTile(100 + i, i + 10, stone);
        setTile(100 - i, i + 10, stone);
    }
}


// Make an Earth-style world
void Map::generateEarth() {
    // Construct generic members of relevent classes, since we don't
    // actually need a new Tile object for every instance of that type
    // Add them to the pointers vector, so they can be deleted later
    Tile *empty = makeTile(TileType::EMPTY);
    Tile *stone = makeTile(TileType::STONE);
    //Tile *dirt = makeTile(TileType::DIRT);
    Tile *magma = makeTile(TileType::MAGMA);
    //Tile *platform = makeTile(TileType::PLATFORM);
    Tile *sandstone = makeTile(TileType::SANDSTONE);
    Tile *mudstone = makeTile(TileType::MUDSTONE);

    // TODO: make this actually do something interesting
    height = 500;
    width = 1000;

    // Create the array of tiles
    tiles = new Tile*[height * width];
    setAll(empty);

    // Make a horizon line for a continent
    int horizon = 200;
    int magmaLevel = 100;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < magmaLevel; y++) {
            setTile(x, y, magma);
            assert(getTile(x, y) -> type == TileType::MAGMA);
        }
        for (int y = magmaLevel; y < horizon; y++) {
            setTile(x, y, stone);
            assert(getTile(x, y) -> type == TileType::STONE);
        }
    }
    // Make a mountain
    int mountainPlace = 200; // TODO make this randomer
    mountain(mountainPlace, horizon, 100);

    // Make a canyon
    int canyonPlace = 800;
    vector<Tile *> fill;
    fill.push_back(sandstone);
    fill.push_back(mudstone);
    canyon(canyonPlace, horizon, 60, -50, fill, -0.7, 3);

    // Make some pillars (desert-style)
    for (int i = 510; i < 600; i += 20) {
        canyon(i, horizon, 10, 20, fill, 0.7, 2);
    }

    // Make hills
    setHills(300, 500, 15, 0.01);
}


// Set all tiles to val
void Map::setAll(Tile *val) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            setTile(i, j, val);
        }
    }
}

// Parts of generating a world

// Start at the top and go down until the tiletype changes
// Returns the height of the top tile of a different type
int Map::findChange(int x, int top) {
    top -= 1;
    TileType val = getTile(x, top) -> type;
    while (top > 0 && getTile(x, top) -> type == val) {
        top--;
    }
    return top;
}

// Set everything from min to y to below and everything from y to max 
// to above, at x
void Map::setTo(int x, int y, int min, int max, Tile *below, Tile *above) {
    assert(min <= y);
    assert(y <= max);
    assert(0 <= min);
    assert(max < height);
    for (int j = min; j < y; j++) {
        setTile(x, j, below);
    }
    for (int j = y; j < max; j++) {
        setTile(x, j, above);
    }
}

// Make an irregular triangle
// This might have a lot in common with canyon and might somehow be combinable
void Map::makeTriangle(int x, int y, int b, int h, 
                vector<Tile *> fill, double mean, double stddev) {
    // direction should be + or - 1, depending on whether the triangle 
    // is point-up
    assert(fill.size() != 0);
    int direction = h / abs(h);
    double slope = 2.0 * (double)h / (double)b;
    b = abs(b);
    double colHeight = 0;
    double random;

    // initialize a normal distribution
    normal_distribution<double> distribution(mean, stddev);

    // Make the height fluctuate randomly
    for(int i = -1 * b / 2; i < 0; i++) {
        random = distribution(generator);
        colHeight = colHeight + random * slope;
        slope = (h - colHeight) / (-1 * i);
        for(int j = 0; j < (int)colHeight; j++) {
            // Pick and set a random tile
            Tile *tile = fill[rand() % fill.size()];
            setTile(x + i, y + (direction * j), tile);
        }
    }
    h = colHeight;
    colHeight = 0;
    for(int i = b / 2; i >= 0; i--) {
        random = distribution(generator);
        colHeight = colHeight + random * slope;
        slope = (h - colHeight) / i;
        for(int j = 0; j < (int)colHeight; j++) {
            // Pick and set a random tile
            Tile *tile = fill[rand() % fill.size()];
            setTile(x + i, y + (direction * j), tile);
        }
    }
}

/* Make a canyon, and fill it with alternating layers of fill
   This can also be used to make triangles with alternating layers of fill
   x and y are the locations of the middle of the base, width is the width
   at the base, height is distance from the peak to the base (should be
   positive for desert-style pillars or negative for canyons), fill is
   a vector of the tile objects that there can be layers of, the slope
   expected from the height and width is divided by mean to get the
   actual slope, and stddev is devietion from an exact triangle. */
void Map::canyon(int x, int y, int width, int height, 
                vector<Tile *> fill, double mean, double stddev) {
    assert(fill.size() != 0);
    // direction should be + or - 1, depending on whether this is a canyon 
    // or a pillar
    int direction = height / abs(height);
    double slope = 2.0 * (double)height / (double)width;
    width = abs(width);
    double left, right, random;
    Tile *tile;

    // initiatialize a normal distribution
    normal_distribution<double> distribution(mean, stddev);

    for(int k = -1; k < 2; k += 2) {
        left = right = (double)width / 2.0;
        for(int i = 0; i < abs(height); i++) {
            // If it has reached 0 thickness at any point or tries to 
            // leave the middle, cap it
            if (left < 1 || right < 1) {
                break;
            }
            random = distribution(generator);
            left -= random / slope;
            random = distribution(generator);
            right -= random / slope;
            // The randomness goes outside that loop to make layers
            tile = fill[rand() % fill.size()];
            for(int j = -1 * (int)left; j < (int)right; j++) {
                setTile(x + (k * j), y + (direction * i), tile);
            }
        }
    }
}

// Make a mountain at x, y
void Map::mountain(int x, int y, int height) {
//TODO: add randomness, make it recursive
// TODO make the limit reasonable and add a recursive else
    vector<Tile *> fill;
    // TODO: find a better source of tile pointers
    Tile *stone = makeTile(TileType::STONE);
    Tile *dirt = makeTile(TileType::DIRT);
    fill.push_back(stone);
    fill.push_back(dirt);
    if (height < 200) {
        makeTriangle(x, y, 2 * height / 3, height, fill, 1, 5);
    }
}

// Code for making hills out of sin waves
// This returns an array of heights
// If it needs to start and stop at a certain height, just add one more
// sine function
// Recommend maxAmp = 15, maxFreq = 0.05, or something like that
// Highly recommend maxFreq be smaller for larger length
// so that it doesn't repeat
double *Map::makeHills(int length, int maxAmp, double maxFreq) {
    assert(length > 0);
    assert(maxAmp > 0);
    double *heights = new double[length];
    // Generate random sign functions and add them together
    for(int k = 0 ; k < 10 ; k++){
        int amp = rand() % maxAmp;
        double freq = ((rand() % 50) / 50.0) * maxFreq;
        double phaseShift = (rand() % 50) / 100.0;
        for(int i = 0 ; i < length ; i ++){
            heights[i] += (amp * sin(i * freq + phaseShift));
        }
    }

    return heights;
}

// Actually place the sinusoidal hills, making sure the heights match
void Map::setHills(int start, int stop, int maxAmp, double maxFreq) {
    assert(stop > start);
    int starty = findChange(start, height);
    int stopy = findChange(stop, height);
    double avgHeight = (starty + stopy) / 2.0;
    // In case it tries to make the hills go off the map
    bool works = false;
    int i = 0;
    double *heights;
    while (works == false && i < 3) {
        works = true;
        // Don't leak the memories of your awful past!
        if (i != 0) {
            delete[] heights;
        }
        heights = makeHills(stop - start, maxAmp, maxFreq);
        // Correct so it's at the right height
        double startAmp = starty - heights[0] - avgHeight;
        double stopAmp = stopy - heights[stop - start - 1] - avgHeight;
        double freq = M_PI / (2.0 * (stop - start));
        double adjust;
        for (int x = start; x < stop; x++) {
            adjust = avgHeight;
            // This way the adjustments add destructively, hopefully
            adjust += startAmp * sin((x - start) * freq + (M_PI / 2.0));
            adjust += stopAmp * sin((x - stop) * freq + (M_PI / 2.0));
            heights[x - start] += adjust;
            if (heights[x - start] >= height || heights[x - start] < 0) {
                works = false;
            }
        }
        i++;
    }
    if (works == false) {
        cerr << "Your hills are just wrong." << endl;
    }

    // Now that we have the heights, actually set the hills to those heights
    // maybe make this a separate function
    for (int x = start; x < stop; x++) {
        Tile *stone = makeTile(TileType::STONE);
        Tile *empty = makeTile(TileType::EMPTY);
        int y = (int)heights[x - start];
        y = max(0, min(y, height - 1));
        int low = min(y, findChange(x, height));
        low = max(low, 0);
        int high = max(y, findChange(x, height) + 1);
        high = min(high, height - 1);
        setTo(x, y, low, high, stone, empty);
    }
}


// Public methods

// Constructor, based on the world type given
Map::Map(WorldType worldType) {
    // Seed the random number generators
    srand(time(NULL));
    generator.seed(time(NULL));

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
    int x = width / 2;
    spawn = Location(x, findChange(x, height));
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

// Returns the tile pointer at x, y
// 0, 0 is the bottom right
Tile *Map::getTile(int x, int y) const {
    return *findPointer(x, y);
}

// Set the tile at x, y equal to val
void Map::setTile(int x, int y, Tile *val) {
    *findPointer(x, y) = val;
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
