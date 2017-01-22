#include <iostream>
#include <fstream> // To read and write files
#include <cassert>
#include <ctime> // To seed the random number generator
#include <cstdlib> // For randomness
#include <cmath> // Because pi and exponentiation
#include "World.hh"

using namespace std;

// Private methods

// Really small helper functions that don't directly change tiles

// Returns a pointer to the tile* at x, y
TileType *World::findPointer(int x, int y, TileType *array) const {
    assert (0 <= x);
    assert (x < width);
    assert (0 <= y);
    assert (y < height);
    return array + (y * width + x);
}

// How to make the different types of worlds

// Make a world good for testing collision detection
void World::generateTest() {
    // TODO: bugfix so it actually makes the shape it should
    
    // Set height and width, and use them to make a tile array
    height = 500;
    width = 500;
    foreground = new TileType[height * width];
    background = new TileType[height * width];
    // Set all tiles to EMPTY, then make some that aren't
    setAll(TileType::EMPTY, foreground);

    // Make the bottom solid
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < 20; j++) {
            setTile(i, j, TileType::MUDSTONE, foreground);
        }
    }

    // Add a couple lines and some platforms
    for (int i = 20; i < 80; i++) {
        setTile(i, 40, TileType::MUDSTONE, foreground);
        setTile(i, 50, TileType::PLATFORM, foreground);
        setTile(i, 60, TileType::MUDSTONE, foreground);
    }

    // Add some diagonal lines
    for (int i = 20; i < 60; i++) {
        setTile(100 + i, i + 10, TileType::MUDSTONE, foreground);
        setTile(100 - i, i + 10, TileType::MUDSTONE, foreground);
    }
}

// Generate a tiny world good for testing world generation
void World::generateSmolTest() {
    width = 50;
    height = 20;

    spawn.x = width / 2;
    spawn.y = height / 2;

    // Create the array of tiles
    foreground = new TileType[width * height];
    background = new TileType[width * height];
    setAll(TileType::EMPTY, foreground);

    // Set a few reference points 
    setTile(0, 0, TileType::RED_SANDSTONE, foreground);
    setTile(0, height - 1, TileType::SANDSTONE, foreground);
    setTile(width - 1, 0, TileType::MUDSTONE, foreground);
    setTile(1, height - 1, TileType::DIRT, foreground);
    
}

// Make an Earth-style world
void World::generateEarth() {
    // TODO: make this actually do something interesting
    height = 500;
    width = 1024;

    // Create the array of tiles
    foreground = new TileType[height * width];
    background = new TileType[height * width];
    setAll(TileType::EMPTY, foreground);

    // Make a horizon line for a continent
    int horizon = 200;
    int magmaLevel = 100;
    for (int x = 0; x < width; x++) {
        setTo(x, 0, magmaLevel, TileType::PERIDOTITE, foreground);
        setTo(x, magmaLevel, horizon, TileType::MUDSTONE, foreground);
    }

    setHills(400, 700, 15, 0.1);
    // Make hills
    // Make a mountain
    int mountainPlace = 200; // TODO make this randomer
    mountain(mountainPlace, horizon, 100);

    // Make a canyon
    int canyonPlace = 750;
    vector<TileType> fill;
    fill.push_back(TileType::SANDSTONE);
    fill.push_back(TileType::RED_SANDSTONE);
    canyon(canyonPlace, horizon, 60, -50, fill, -0.7, 2);

    // Make some pillars (desert-style)
    for (int i = 800; i < 950; i += 20) {
        canyon(i, horizon, 10, 20, fill, 0.7, 2);
    }

    // Add a layer of dirt
    vector<TileType> justdirt;
    justdirt.push_back(TileType::DIRT);
    // Constructs a vector of length width with every value set to 5
    vector<double> heights (width, 5.0);
    vector<double> perlin = noise(20, width / 16, 16);
    heights = merge(0, width, heights, 0, width, perlin);
    addHeights(0, width, heights, justdirt, height, foreground);
}


// Set all tiles to val
void World::setAll(TileType val, TileType *array) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            setTile(i, j, val, array);
        }
    }
}

// Parts of generating a world

// Start at the top and go down until the tiletype changes
// Returns the height of the top tile of a different type
int World::findChange(int x, int top, TileType *array) const {
    assert(0 <= x);
    assert(x < width);
    assert(0 < top);
    assert(top <= height);
    top -= 1;
    TileType val = getTile(x, top, array);
    while (top > 0 && getTile(x, top, array) == val) {
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
vector<double> World::merge(int start1, int stop1, 
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
void World::setTo(int x, int y1, int y2, TileType tile, TileType *array) {
    vector<TileType> fill;
    fill.push_back(tile);
    setTo(x, y1, y2, fill, array);
}

// Set everything from y1 to y2 to something picked randomly from fill, at x
void World::setTo(int x, int y1, int y2, const vector<TileType> &fill, 
        TileType *array) {
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
        setTile(x, j, fill[index], array);
    }
}

// From x = start to x = stop, add heights[x - start] of a randomly 
// selected Tile* from fill, above the line given by findChange(x, top, array)
void World::addHeights(int start, int stop, const vector<double> &heights,
                const vector<TileType> &fill, int top, TileType *array) {
    assert(0 <= start);
    assert(start <= stop);
    assert(stop <= width);
    assert(0 < top);
    assert(top <= height);

    for (int i = 0; i < stop - start; i++) {
        int x = i + start;
        int y = findChange(x, top, array) + 1;
        int newy = y + (int)heights[i];
        if (0 > newy) {
            cerr << "Tried to set height to " << newy << endl;
            newy = 0;
        }
        else if (newy >= height) {
            cerr << "Tried to set height to " << newy << endl;
            newy = height - 1;
        }
        setTo(x, y, newy, fill, array);
    }
}

// Move the horizon line from findChange(x, top, array) to heights[x - start]
void World::setHeights(int start, int stop, const vector<double> &heights, 
        const vector<TileType> &above, const vector<TileType> &below, 
        int top, TileType *array) {
    assert(0 <= start);
    assert(start <= stop);
    assert(stop <= width);
    assert(0 < top);
    assert(top <= height);

    vector<TileType> fill;
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
        int oldy = findChange(x, top, array);
        oldy++;
        // Pick which fill to set
        if (oldy > newy) {
            fill = above;
        }
        else {
            fill = below;
        }
        setTo(x, newy, oldy, fill, array);
        // assert (findChange(x, top, array) == newy - 1);
    }
}

// Linear interpolator
double World::lerp(double lo, double hi, double t) const {
    return lo * (1 - t) + hi * t;
}

// Generate Perlin noise between 0 and range
// The length of the array is times * wavelength
vector<double> World::noise(double range, int times, int wavelength) const {
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

// Generate a heightmap recursively by midpoint displacement
// length is best as (a power of 2) + 1, start, end, and mid are the 
// heights at those places. variance is the coefficient of the random 
// number added. exp is the  number that the variance is multiplied by 
// every  iteration. For best results,
// use a  number between 0 and 1. An exp closer to 0 will make smoother 
// terrain, one closer to 1 will make more jagged terrain.
vector<double> World::midpointDisplacement(int length, double start, double end,
    double mid, double exp, double variance) {
    // Initialize the heightmap
    vector<double> heights;
    heights.resize(length);

    // If exp is greater than 1, warn the user
    if (exp < -1 || exp > 1) {
        cerr << "Warning: calling midpointDisplacement with large exp.\n";
    }

    // Set the start, end, and midpoint
    assert(length > 2);
    heights[0] = start;
    heights[length - 1] = end;
    heights[length / 2] = mid;

    // Solve the base case
    if (length == 3) {
        return heights;
    }

    // Recurse! (This isn't very efficient.)

    // Have randomness between -1 and 1
    uniform_real_distribution<double> distribution(-1.0, 1.0);

    // Find the rest of the first half
    int newLength = length / 2 + 1;
    double newMid = (start + mid) / 2 + variance * distribution(generator);
    if(newLength > 2) {
        vector<double> first = midpointDisplacement(newLength, start, mid, 
            newMid, exp, variance * exp);
        assert(heights.size() > first.size());
        assert(heights[0] == first[0]);
        assert(first[first.size() - 1] == mid);
        for (unsigned int i = 0; i < first.size(); i++) {
            heights[i] = first[i];
        }
    }

    // Find the rest of the second half
    newLength = (length + 1) / 2; // Different because rounding
    if (newLength > 2) {
        newMid = (end + mid) / 2 + variance * distribution(generator);
        vector<double> second = midpointDisplacement(newLength, mid, end, 
            newMid, exp, variance * exp);
        assert(heights.size() > second.size());
        assert(heights[length / 2] == second[0]);
        assert(heights[length - 1] == second[second.size() - 1]);
        for (unsigned int i = 0; i < second.size(); i++) {
            heights[length / 2 + i] = second[i];
        }
    }

    return heights;

}

// Make an irregular triangle
vector<double> World::makeTriangle(int b, int h, double mean, double stddev) {
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
            // This number starts at 0 and keeps increasing by 1
            int index = (i * k) + b - ((k + 1) / 2);
            assert(index < 2 * b);
            assert(index >= 0);
            // Just in case values get wierd
            if (abs(colHeight) > 2 * abs(h)) {
                cerr << "colHeight " << colHeight << " set to " << h << endl;
                colHeight = h;
            }
            heights[index] = colHeight;
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
void World::canyon(int x, int y, int width, int height, 
        const vector<TileType> &fill, double mean, double stddev) {
    assert(fill.size() != 0);
    // direction should be + or - 1, depending on whether this is a canyon 
    // or a pillar
    int direction = height / abs(height);
    double slope = 2.0 * (double)height / (double)width;
    width = abs(width);
    double left, right, random;
    TileType tile;

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
                setTile(x + (k * j), y + (direction * i), tile, foreground);
            }
        }
    }
}

// Make a mountain at x, y
void World::mountain(int x, int y, int h) {
//TODO: add randomness, make it recursive
// TODO make the limit reasonable and add a recursive else
    vector<TileType> below, above;
    above.push_back(TileType::EMPTY);
    below.push_back(TileType::MUDSTONE);
    vector<double> heights;
    int w = 4 * h / 3;
    if (h < 200) {
        heights = midpointDisplacement(2 * w, (double)y, (double)y, 
            (double)(h + y), 0.3, (double)h);
    }

    setHeights(x - w, x + w, heights, above, below, height, foreground);
}

// Code for making hills out of sin waves
// This returns an array of heights
// If it needs to start and stop at a certain height, just add one more
// sine function
// Recommend maxAmp = 15, maxFreq = 0.05, or something like that
// Highly recommend maxFreq be smaller for larger length
// so that it doesn't repeat
vector<double> World::makeHills(int length, int maxAmp, double maxFreq) const {
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
            setTile(i, n + 200, TileType::PERIDOTITE, foreground);
            */
        }
    }

    return heights;
}

// Actually place the sinusoidal hills, making sure the heights match
void World::setHills(int start, int stop, int maxAmp, double maxFreq) {
    assert(stop > start);
    int starty = findChange(start, height, foreground);
    starty = max(0, starty);
    int stopy = findChange(stop, height, foreground);
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
    vector<TileType> above(1, TileType::EMPTY);
    vector<TileType> below(1, TileType::MUDSTONE);
    setHeights(start, stop, heights, above, below, height, foreground);
}


// Public methods

// Constructor, based on the world type given
World::World(WorldType worldType) {
    // Seed the random number generators
    srand(time(NULL));
    generator.seed(time(NULL));

    // Run the appropriate function
    switch(worldType) {
        case WorldType::TEST : 
            generateTest();
            break;
        case WorldType::SMOLTEST :
            generateSmolTest();
            break;
        case WorldType::EARTH :
            generateEarth();
            break;
        default :
            cerr << "Maybe I'll implement that later." << endl;
    }

    spawn.x = width / 2;
    // I should be careful to make sure there are never cloud cities or 
    // floating islands or whatever directly above the spawn point, so the
    // player doesn't die of fall damage every time they respawn.
    spawn.y = max(0, findChange(spawn.x, height, foreground)) + 4;
}

// Destructor
World::~World() {
    // Delete the map
    delete[] foreground;
    delete[] background;
}


// Return the height of the map, in number of tiles
int World::getHeight() const {
    return height;
}

// Return the width of the map, in number of tiles
int World::getWidth() const {
    return width;
}

// Return the default spawn point
Location World::getSpawn() const {
    return spawn;
}

TileType World::getTile(int x, int y, TileType *array) const {
    return *findPointer(x, y, array);
}

// Set the tile at x, y equal to val
void World::setTile(int x, int y, TileType val, TileType *array) {
    *findPointer(x, y, array) = val;
}

// Write the map to a file
void World::save(const string &filename) const {
    // Saves in .bmp file format in black and white
    ofstream outfile;
    outfile.open(filename);

    // Write an informative header
    outfile << "#Map\n" << width << " " << height << "\n";
    outfile << spawn.x << " " << spawn.y << "\n";
    // Write tile values

    // For keeping track of a lot of the same tile in a row
    int count = 0;
    TileType last = foreground[0];
    TileType current;

    // Treat foreground as the 1D array it is.
    for (int index = 0; index < height * width; index++) {
        current = foreground[index];
        if(current != last) {
            outfile << count << " ";
            outfile << (int)last << " ";
            last = current;
            count = 1;
        }
        else {
            count ++;
        } 
    }
    
    // Write the last set of numbers
    outfile << count << " " << (int)last << " ";
    outfile.close();
}
