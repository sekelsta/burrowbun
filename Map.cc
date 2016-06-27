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
    Tile *tile = new Tile(val, pointers.size());
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
    Tile *dirt = makeTile(TileType::DIRT);
    Tile *magma = makeTile(TileType::MAGMA);
    //Tile *platform = makeTile(TileType::PLATFORM);
    Tile *sandstone = makeTile(TileType::SANDSTONE);
    Tile *mudstone = makeTile(TileType::MUDSTONE);

    // TODO: make this actually do something interesting
    height = 500;
    width = 1024;

    // Create the array of tiles
    tiles = new Tile*[height * width];
    setAll(empty);

    // Make a horizon line for a continent
    int horizon = 200;
    int magmaLevel = 100;
    for (int x = 0; x < width; x++) {
        setTo(x, 0, magmaLevel, magma);
        setTo(x, magmaLevel, horizon, stone);
    }

    setHills(400, 700, 15, 0.1);
    // Make hills
    // Make a mountain
    int mountainPlace = 200; // TODO make this randomer
    mountain(mountainPlace, horizon, 100);

    // Make a canyon
    int canyonPlace = 750;
    vector<Tile *> fill;
    fill.push_back(sandstone);
    fill.push_back(mudstone);
    canyon(canyonPlace, horizon, 60, -50, fill, -0.7, 2);

    // Make some pillars (desert-style)
    for (int i = 800; i < 950; i += 20) {
        canyon(i, horizon, 10, 20, fill, 0.7, 2);
    }

    // Add a layer of dirt
    vector<Tile *> justdirt;
    justdirt.push_back(dirt);
    // Constructs a vector of length width with every value set to 5
    vector<double> heights (width, 5.0);
    vector<double> perlin = noise(20, width / 16, 16);
    heights = merge(0, width, heights, 0, width, perlin);
    addHeights(0, width, heights, justdirt, height);
}


// Set all tiles to val
void Map::setAll(Tile* const &val) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            setTile(i, j, val);
        }
    }
}

// Parts of generating a world

// Start at the top and go down until the tiletype changes
// Returns the height of the top tile of a different type
int Map::findChange(int x, int top) const {
    assert(0 <= x);
    assert(x < width);
    assert(0 < top);
    assert(top <= height);
    top -= 1;
    TileType val = getTile(x, top) -> type;
    while (top > 0 && getTile(x, top) -> type == val) {
        top--;
    }
    // If there was no change, return -1
    if (top == 0) {
        return -1;
    }
    // Otherwise, return the place where there was a change
    return top;
}

// Merge two arrays, adding them wherever they overlap 
vector<double> Map::merge(int start1, int stop1, 
                    const vector<double> &heights1, int start2, 
                    int stop2, const vector<double> &heights2) const {
    assert(0 <= start1);
    assert(0 <= start2);
    assert(stop1 <= width);
    assert(stop2 <= width);
    assert(start1 <= stop1);
    assert(start2 <= stop2);
    int start = min(start1, start2);
    int length = max(stop1, stop2) - start;
    vector<double> merged;
    merged.resize(length);
    for (int i = 0; i < stop1 - start1; i++) {
        merged[start1 - start + i] = heights1[i];
    }
    for (int i = 0; i < stop2 - start2; i++) {
        merged[start2 - start + i] += heights2[i];
    }
    return merged;
}

// Set everything from y1 to y2 to tile
void Map::setTo(int x, int y1, int y2, Tile* const &tile) {
    vector<Tile *> fill;
    fill.push_back(tile);
    setTo(x, y1, y2, fill);
}

// Set everything from y1 to y2 to something picked randomly from fill, at x
void Map::setTo(int x, int y1, int y2, const vector<Tile *> &fill) {
    int miny = min(y1, y2);
    int maxy = max(y1, y2);
    assert(0 <= miny);
    assert(maxy < height);

    // Store the size of the vector
    int size = fill.size();
    int index;
    assert(size != 0);

    for (int j = miny; j < maxy; j++) {
        index = rand() % size;
        setTile(x, j, fill[index]);
    }
}

// From x = start to x = stop, add heights[x - start] of a randomly 
// selected Tile* from fill, above the line given by findChange(x, top)
void Map::addHeights(int start, int stop, const vector<double> &heights,
                const vector<Tile *> &fill, int top) {
    assert(0 <= start);
    assert(start <= stop);
    assert(stop <= width);
    assert(0 < top);
    assert(top <= height);

    for (int i = 0; i < stop - start; i++) {
        int x = i + start;
        int y = findChange(x, top) + 1;
        int newy = y + (int)heights[i];
        if (0 > newy) {
            cerr << "Tried to set height to " << newy << endl;
            newy = 0;
        }
        else if (newy >= height) {
            cerr << "Tried to set height to " << newy << endl;
            newy = height - 1;
        }
        setTo(x, y, newy, fill);
    }
}

// Move the horizon line from findChange(x, top) to heights[x - start]
void Map::setHeights(int start, int stop, const vector<double> &heights, 
                    const vector<Tile *> &above, 
                    const vector<Tile *> &below, int top) {
    assert(0 <= start);
    assert(start <= stop);
    assert(stop <= width);
    assert(0 < top);
    assert(top <= height);

    vector<Tile *> fill;
    for (int i = 0; i < stop - start; i++) {
        int x = i + start;
        int newy = (int)heights[i];
        newy++;
        // TODO: this is repeated code
        if (0 > newy) {
            cerr << "Tried to set height to " << newy << endl;
            newy = 0;
        }
        else if (height <= newy) {
            cerr << "Tried to set height to " << newy << endl;
            newy = height - 1;
        }
        int oldy = findChange(x, top);
        oldy++;
        // Pick which fill to set
        if (oldy > newy) {
            fill = above;
        }
        else {
            fill = below;
        }
        setTo(x, newy, oldy, fill);
        assert (findChange(x, top) == newy - 1);
    }
}

// Linear interpolator
double Map::lerp(double lo, double hi, double t) const {
    return lo * (1 - t) + hi * t;
}

// Generate Perlin noise between 0 and range
// The length of the array is times * wavelength
vector<double> Map::noise(double range, int times, int wavelength) const {
    assert(times >= 0);
    assert(wavelength > 0);
    assert(range > 0);
    int length = times * wavelength;
    vector<double> values;
    values.resize(length);
    // Add white noise
    for (int i = 0; i < length; i += wavelength) {
        values[i] = (double)(rand() % 512) / 512.0 * range; 
    }
    // Interpolate
    for (int i = 0; i < length; i++) {
        // Calculate distance from a seed value
        double t = (double)(i % wavelength) / (double)wavelength;
        // Remap t using smoothstep so it's not just a line
        t = t * t * (3 - 2 * t);
        // Calculate the value
        int lo = (i / wavelength) * wavelength;
        int hi = (lo + wavelength) % length;
        values[i] = lerp(values[lo], values[hi], t);
    }
    return values;
}

// Make an irregular triangle
vector<double> Map::makeTriangle(int b, int h, double mean, double stddev) {
    b = abs(b / 2);
    vector<double> heights;
    heights.resize(2 * b);
    double slope = (double)h / (double)b;
    double colHeight = 0;
    double random;

    // initialize a normal distribution
    normal_distribution<double> distribution(mean, stddev);

    // Make the height fluctuate randomly
    for (int k = -1; k < 2; k += 2) {
        colHeight = 0;
        for(int i = b; i > 0; i--) {
            random = distribution(generator);
            colHeight += random * slope;
            slope = ((double)(h - colHeight)) / ((double)i);
            heights[((i * k) + b)] = colHeight;
            // Just in case values get wierd
            if (abs(colHeight) > 2 * abs(h)) {
                cerr << "colHeight " << colHeight << " set to " << h << endl;
                colHeight = h;
            }
        }
    }
    // Add the middle
    heights[b] = h;
    return heights;
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
                    const vector<Tile *> &fill, 
                    double mean, double stddev) {
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
void Map::mountain(int x, int y, int h) {
//TODO: add randomness, make it recursive
// TODO make the limit reasonable and add a recursive else
    vector<Tile *> below, above;
    // TODO: find a better source of tile pointers
    Tile *stone = makeTile(TileType::STONE);
    Tile *dirt = makeTile(TileType::DIRT);
    Tile *empty = makeTile(TileType::EMPTY);
    above.push_back(empty);
    below.push_back(stone);
    below.push_back(dirt);
    vector<double> heights;
    int w = 2 * h / 3;
    if (h < 200) {
        heights = makeTriangle(2 * w, h, 1, 1);
    }
    for (int i = 0; i < 2 * w; i++) {
        heights[i] += y;
    }
    setHeights(x - w, x + w, heights, above, below, height);
}

// Code for making hills out of sin waves
// This returns an array of heights
// If it needs to start and stop at a certain height, just add one more
// sine function
// Recommend maxAmp = 15, maxFreq = 0.05, or something like that
// Highly recommend maxFreq be smaller for larger length
// so that it doesn't repeat
vector<double> Map::makeHills(int length, int maxAmp, double maxFreq) const {
    assert(length > 0);
    assert(maxAmp > 0);
    vector<double> heights;
    heights.resize(length);
    // Generate random sign functions and add them together
    for(int k = 0 ; k < 10 ; k++){
        int amp = rand() % maxAmp;
        double freq = ((rand() % 512) / 512.0) * maxFreq;
        double phaseShift = ((rand() % 512) / 512.0) * M_PI;
        for(int i = 0 ; i < length ; i ++){
            heights[i] += (amp * sin(i * freq + phaseShift));
            /*int n = (amp * sin(i * freq + phaseShift));
            heights[i] += n;
            // Code to view what sin functions are involved
            Tile *tile = makeTile(TileType::MAGMA);
            setTile(i, n + 200, tile);
            */
        }
    }

    return heights;
}

// Actually place the sinusoidal hills, making sure the heights match
void Map::setHills(int start, int stop, int maxAmp, double maxFreq) {
    assert(stop > start);
    int starty = findChange(start, height);
    starty = max(0, starty);
    int stopy = findChange(stop, height);
    stopy = max(0, stopy);
    double avgHeight = (starty + stopy) / 2.0;
    vector<double> heights = makeHills(stop - start, maxAmp, maxFreq);

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
    }

    // Now that we have the heights, actually set the hills to those heights
    vector<Tile *> above(1, makeTile(TileType::EMPTY));
    vector<Tile *> below(1, makeTile(TileType::STONE));
    setHeights(start, stop, heights, above, below, height);
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
    spawn = Location(x, max(0, findChange(x, height)));
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
void Map::setTile(int x, int y, Tile* const &val) {
    *findPointer(x, y) = val;
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
            outfile << (int)(getTile(i, j) -> type) << " ";
        }
        outfile << "\n";
    }
    outfile.close();
}
