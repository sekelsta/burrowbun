#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime> // To seed the random number generator
#include <cstdlib> // For randomness
#include <math.h> // Because pi
#include <tgmath.h> // for exponentiation
#include "Map.hh"
#include "Boulder.hh"
#include "../version.hh"
#include "../entity/DroppedItem.hh"
#include "../action/ItemMaker.hh"
#include <queue>

#define MAX_LIGHT_DEPTH 5

using namespace std;

Tile *Map::newTile(TileType val) {
    Tile *tile = nullptr;
    /* If it's a boulder, make a boulder. */
    if ((unsigned int)TileType::FIRST_BOULDER <= (unsigned int)val
            && (unsigned int)val <= (unsigned int)TileType::LAST_BOULDER) {
        tile = new Boulder(val, path);
    }
    /* Otherwise it's just a plain tile. */
    else {
        tile = new Tile(val, path);
        assert((unsigned int)TileType::FIRST_TILE <= (unsigned int)val);
        assert((unsigned int)val <= (unsigned int)TileType::LAST_PURE_TILE);
    }

    assert(tile != nullptr);

    /* Add it to the pointers at index (int) val. */
    if (pointers.size() <= (unsigned int)val) {
        pointers.resize((unsigned int)val + 1);
        assert(pointers.back() == nullptr);
    }
    pointers[(unsigned int)val] = tile;

    return tile;
}

void Map::randomizeSprites() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            chooseSprite(i, j);
        }
    }
}

void Map::chooseSprite(int x, int y) {
    Location place;
    place.x = x;
    place.y = y;
    place.layer = MapLayer::FOREGROUND;
    Location spritePlace = getTile(place) -> getSpritePlace(*this, place);
    findPointer(x, y) -> foregroundSprite 
        = SpaceInfo::toSpritePlace(spritePlace);
    place.layer = MapLayer::BACKGROUND;
    spritePlace = getTile(place) -> getSpritePlace(*this, place);
    findPointer(x, y) -> backgroundSprite 
        = SpaceInfo::toSpritePlace(spritePlace);
}

bool Map::isBesideTile(int x, int y, MapLayer layer) {
    /* Check at this place. */
    if (getTile(x, y, layer) -> type != TileType::EMPTY
            && getTile(x, y, layer) -> type != TileType::WATER) {
        return true;
    }

    /* Check each tile next to it. */
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            /* Don't check the status of tiles off the edge of the map, 
            or if it's part of a diagonal line through the center tile. */
            if (isOnMap(x + i, y + j) && i != j && i != -1 * j
                    && getTile(x+i, y+j, layer) -> type != TileType::EMPTY
                    && getTile(x+i, y+j, layer) -> type != TileType::WATER) {
                return true;
            }
        }
    }

    return false;

}

inline bool Map::isSky(int x, int y) {
    assert(y >= 0);
    assert(y < height);
    x = wrapX(x);
    return (getForeground(x, y) -> getIsSky()
        && getBackground(x, y) -> getIsSky());
}

void Map::effectLight(int x, int y, const Light &l, 
        vector<vector<int>> &current) {
    int mld = MAX_LIGHT_DEPTH;
    assert(current.size() == 2 * MAX_LIGHT_DEPTH + 1);
    assert(current[0][0] == -1);
    assert(current[mld][mld] == -1);

    /* Do pattern like breadth-first search. */
    queue<Location> unvisited;
    unvisited.push({0, 0, MapLayer::FOREGROUND});
    DLight lback = getBackground(x, y) -> getAbsorbed();
    current[MAX_LIGHT_DEPTH][MAX_LIGHT_DEPTH] = max(
        getForeground(x, y) -> getAbsorbed().r, lback.r * lback.a);

    while (!unvisited.empty()) {
        Location loc = unvisited.front();
        unvisited.pop();
        /* Stop if we've gotten to the edge of where the light should reach. */
        if (loc.x >= mld || loc.y >= mld || loc.x <= -1 * mld 
                || loc.y <= -1 * mld) {
            continue;
        }

        int next = current[MAX_LIGHT_DEPTH + loc.x][MAX_LIGHT_DEPTH + loc.y];
        /* Stop if we've gotten to the edge of where the light reaches. */
        if (next >= 64) {
            continue;
        }

        assert(next != -1);

        Tile *fore = getTile(x + loc.x, y + loc.y, MapLayer::FOREGROUND);
        Tile *back = getTile(x + loc.x, y + loc.y, MapLayer::BACKGROUND);
        
        int opacity = max(fore -> getAbsorbed().r, 
            back -> getAbsorbed().r * back -> getAbsorbed().a);
        int edge = next + opacity;

        vector<Location> edges = {{-1, 0, (MapLayer)0}, {0, -1, (MapLayer)0}, 
            {1, 0, (MapLayer)0}, {0, 1, (MapLayer)0}};
        vector<Location> corners = {{-1, -1, (MapLayer)0}, {1, -1, (MapLayer)0},
            {-1, 1, (MapLayer)0}, {1, 1, (MapLayer)0}};
        for (int i = 0; i < 4; i++) {
            int ix = mld + loc.x + edges[i].x;
            int iy = mld + loc.y + edges[i].y;
            if (current[ix][iy] == -1) {
                current[ix][iy] = edge;
                unvisited.push({ix - mld, iy - mld, (MapLayer)0});
            }
            else {
                current[ix][iy] = min(current[ix][iy], edge);
            }
            int cx = mld + loc.x + corners[i].x;
            int cy = mld + loc.y + corners[i].y;
            Tile *cfore = getForeground(x + cx - mld, y + cy - mld);
            Tile *cback = getBackground(x + cx - mld, y + cy - mld);
        
            int copacity = max(cfore -> getAbsorbed().r, 
                cback -> getAbsorbed().r * cback -> getAbsorbed().a);
            int ccorner = next + 1.41 * (copacity + opacity) / 2.0;
            if (current[cx][cy] == -1) {
                current[cx][cy] = ccorner;
                unvisited.push({cx - mld, cy - mld, (MapLayer)0});
            }
            else {
                current[cx][cy] = min(current[cx][cy], ccorner);
            }
        }
    }
}


void Map::addLight(int x, int y, vector<vector<int>> &current, 
        const Light &l) {
    for (unsigned int i = 0; i < current.size(); i++) {
        for (unsigned int j = 0; j < current[i].size(); j++) {
            int xi = x + i - current.size() / 2;
            int yi = y + j - current[i].size() / 2;
            if (current[i][j] != -1) {
                findPointer(xi, yi) -> light.setmax(
                    l.times(getExpLight(current[i][j])));
                current[i][j] = -1;
            }
        }
    }
}

void Map::setLight(int xstart, int ystart, int xstop, int ystop) {
    int mld = MAX_LIGHT_DEPTH;
    bool done = true;
    int xlookstart = wrapX(xstart - MAX_LIGHT_DEPTH);
    int xlookstop = wrapX(xstop + MAX_LIGHT_DEPTH);
    int ylookstart = min(max(0, ystart - MAX_LIGHT_DEPTH), height - 1);
    int ylookstop = min(max(0, ystop + MAX_LIGHT_DEPTH), height - 1);
    set<Location> toCheck;
    /* Loop through and make sure none need to be updated. */
    for (int i = xlookstart; i < xlookstop; i++) {
        for (int j = ylookstart; j < ylookstop; j++) {
            bool change = false;
            if (findPointer(i, j) -> lightRemoved) {
                done = false;
                findPointer(i, j) -> lightRemoved = false;

                /* The map itself will be used to keep track of the summation 
                of lights we have calculated so far. */
                for (int x = i - 1 * mld; x <= i + mld; x++) {
                    for (int y = j - 1 * mld; y <= j + mld; y++) {
                        findPointer(x, y) -> light = Light(0, 0, 0, 0);
                    }
                }

                /* Add anything that could affect those to the list of lights to
                calculate. */
                for (int x = i - 2 * mld; x <= i + 2 * mld; x++) {
                    for (int y = j - 2 * mld; y <= j + 2 * mld; y++) {
                        if (isOnMap(x, y)) {
                            toCheck.insert({x, y, (MapLayer)0});
                        }
                    }
                }
                
            }
            if (findPointer(i, j) -> lightAdded) {
                change = true;
                findPointer(i, j) -> lightAdded = false;
            }
            if (i >= xstart && i < xstop && j >= ystart && j < ystop
                    && !findPointer(i, j) -> isLightUpdated) {
                change = true;
                findPointer(i, j) -> isLightUpdated = true;
            }

            if (change) {
                done = false;
                /* Add anything that could affect those to the list of lights to
                calculate. */
                for (int x = i - mld; x <= i + mld; x++) {
                    for (int y = j - mld; y <= j + mld; y++) {
                        if (isOnMap(x, y)) {
                            toCheck.insert({x, y, (MapLayer)0});
                        }
                    }
                }
            }
        }
    }
    if (done) {
        return;
    }

    vector<vector<int>> current;
    current.resize(2 * MAX_LIGHT_DEPTH + 1);
    for (unsigned int i = 0; i < current.size(); i++) {
        current[i].resize(2 * MAX_LIGHT_DEPTH + 1, -1);
    }

    // TODO: Make an array to keep track of the summation of darkness we have
    // so far.

    /* Loop over the map looking for light sources. */
    set<Location>::iterator it;
    for (it = toCheck.begin(); it != toCheck.end(); it++) {
        int x = it -> x;
        int y = it -> y;
        assert(isOnMap(x, y));
        if (isSky(x, y)) {
            findPointer(x, y) -> light = getSkyLight();
            bool used = false;
            /* If there's a non-sky tile next to it, this sky is a light
            source. */
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    if (isOnMap(x + i, y + j) && !isSky(x + i, y + j)
                            /* Avoid recaclutating. */
                            && !used) {
                        used = true;
                        effectLight(x, y, Light(0, 0, 0, 255), current);
                        addLight(x, y, current, Light(0, 0, 0, 255));
                    }
                }
            }
        }

        /* If this tile is a light source */
        Light emitted = getTile(x, y, MapLayer::FOREGROUND) -> getEmitted();
        if (emitted.r != 0 || emitted.g != 0 || emitted.b != 0) {
            effectLight(x, y, emitted, current);
            addLight(x, y, current, emitted);
        }

        // possible TODO: light sources in the background (maybe just no)
        // TODO: darksources
        // TODO: movable lights
    }

}

void Map::updateNear(int x, int y) {
    findPointer(wrapX(x), y) -> isLightUpdated = false;
    /* Value that takes into account x-wrapping of the map. */
    Location fore;
    Location back;
    fore.layer = MapLayer::FOREGROUND;
    back.layer = MapLayer::BACKGROUND;
    for (int i = -1; i < 2; i++) {
        fore.x = wrapX(x + i);
        back.x = wrapX(x + i);
        for (int j = -1; j < 2; j++) {
            if (isOnMap(wrapX(x + i), y + j)) {

                fore.y = y + j;
                back.y = y + j;
                /* Update the tiles. */
                addToUpdate(fore);
                addToUpdate(back);
                /* Update the sprites. */
                setSprite(fore, getTile(fore) -> updateSprite(*this, fore));
                setSprite(back, getTile(back) -> updateSprite(*this, back));
            }
        }
    }
}


int Map::bordering(const Location &place) {
    EdgeType thisEdge = getTile(place) -> getEdge();
    /* TODO: when rendering of liquids is added, see if this is actually what
    I want to happen. */
    if (thisEdge == EdgeType::LIQUID) {
        return 0;
    }
    /* Special case for torches. They want to act like they are next to dirt, 
    but they do not want dirt to act like it is next to them. */
    if (thisEdge == EdgeType::TORCH) {
        thisEdge = EdgeType::SOLID;
    }

    int col = 0;
    if (place.y != height - 1 
            && getTile(place.x, place.y+1, place.layer) -> getEdge()
            != thisEdge) {
        col += 1;
    }
    /* WrapX is called so it matches up with the tile on the other side
    of the map, which it's next to when it wraps around. */
    if (getTile(place.x+1, place.y, place.layer) -> getEdge() != thisEdge) {
        col += 2;
    }
    if (place.y != 0 
            && getTile(place.x, place.y-1, place.layer) -> getEdge() 
            != thisEdge) {
        col += 4;
    }
    if (getTile(place.x-1, place.y, place.layer) -> getEdge() != thisEdge) {
        col += 8;
    }
    return col;
}

void Map::saveLayer(MapLayer layer, ofstream &outfile) const {
    // For keeping track of a lot of the same tile in a row
    int count = 0;
    TileType current, last;
    /* Just to get rid of the "may be uninitialized" compile warning.
    (It can't be uninitialized because of the assertions.) */
    last = TileType::EMPTY;

    assert(height != 0);
    assert(width != 0);

    // Treat the array as the 1D array it is.
    for (int index = 0; index < height * width; index++) {
        /* Get a tiletype from the correct layer. */
        if (layer == MapLayer::FOREGROUND) {
            current = tiles[index].foreground;
        }
        else {
            assert(layer == MapLayer::BACKGROUND);
            current = tiles[index].background;
        }
        if(index != 0 && current != last) {
            outfile << count << " ";
            outfile << (int)last << " ";
            count = 1;
        }
        else {
            count ++;
        }
        last = current;
    }

    // Write the last set of numbers
    outfile << count << " " << (int)last << " ";
}

void Map::save(std::string filename) const {
    // Saves in .bmp file format in black and white
    std::ofstream outfile;
    outfile.open(filename);

    // Write an informative header
    outfile << "#Map\n" << MAJOR << " " << MINOR << " " << PATCH << "\n";
    outfile << width << " " << height << "\n";
    outfile << spawn.x << " " << spawn.y << "\n";
    outfile << seed << "\n";
    // Write tile values
    outfile << "#Foreground\n";
    saveLayer(MapLayer::FOREGROUND, outfile);

    /* Time for the background. */
    outfile << "\n#Background\n";
    saveLayer(MapLayer::BACKGROUND, outfile);

    /* Biome information. */
    outfile << "\n#Biomes\n";
    assert(biomes.size() == (unsigned int)(biomesWide * biomesHigh));
    assert(biomesWide != 0);
    assert(biomesHigh != 0);

    int count = 0;
    BiomeType current;
    /* Some random value so I don't get uninitialized value compile warnings.
    Because of the if statement, it won't be uninitialized if it's used, as 
    long as the map has nonzero size (which is asserted).
    */
    BiomeType last = BiomeType::GRASSLAND;
    for (int i = 0; i < biomesWide * biomesHigh; i++) {
        current = biomes[i].biome;
        if (i != 0 && current != last) {
            outfile << count << " " << (int)last << " ";
            count = 1;
        }
        else {
            count++;
        }
        last = current;
    }

    /* One last set of biome values. */
    outfile << count << " " << (int)last << " ";

    /* All the other data. */
    outfile << "\n#Other\n";
    for (int i = 0; i < width * height; i++) {
        outfile << (int)tiles[i].foregroundSprite << " ";
        outfile << (int)tiles[i].backgroundSprite << " ";
    }

    outfile.close();
}

void Map::loadLayer(MapLayer layer, ifstream &infile) {
    int index = 0;
    int count, tile;
    TileType current;
    while (index < height * width) {
        infile >> count >> tile;
        current = (TileType)tile;
        for (int i = 0; i < count; i++) {
            assert(index < width * height);
            if (layer == MapLayer::FOREGROUND) {
                tiles[index].foreground  = current;
            }
            else {
                assert(layer == MapLayer::BACKGROUND);
                tiles[index].background = current;
            }
            ++index;
        }
    }
    assert(getForeground(0, 0) -> type == tiles[0].foreground);
}

// Constructor
Map::Map(string filename, int tileWidth, int tileHeight, string p) : 
        TILE_WIDTH(tileWidth), TILE_HEIGHT(tileHeight) {
    /* It's the 0th tick. */
    tick = 0;
    ifstream infile(filename);
    path = p;

    exps.resize(MAX_OPACITY, 0);

    /* Create a tile object for each type. */
    for (int i = 0; i <= (int)TileType::LAST_TILE; i++) {
        newTile((TileType)i);
    }

    /* Check that the file could be opened. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }

    /* Check that the header is #Map, just in case we were given an entirely
    wrong file. */
    string header;
    infile >> header;
    if (header != "#Map") {
        cerr << filename << " doesn't say it's a map." << "\n";
    }

    string major;
    string minor;
    string patch;
    infile >> major >> minor >> patch;
    if (stoi(major) != MAJOR || stoi(minor) != MINOR) {
        cerr << "Warning: This map was written with version ";
        cerr << major << "." << minor << "." << patch << ", ";
        cerr << "but this software is version " << MAJOR << ".";
        cerr << MINOR << "." << PATCH << "." << " The save format may ";
        cerr << "have changed.\n";
    }

    /* Read in the things. */
    infile >> width >> height;
    setWidth(width);
    setHeight(height);
    tiles = new SpaceInfo[width * height];
    biomes.resize(biomesWide * biomesHigh);
    infile >> spawn.x >> spawn.y;
    infile >> seed;

    infile >> header;
    if (header != "#Foreground") {
        cerr << "Couldn't load foreground!\n";
    }

    loadLayer(MapLayer::FOREGROUND, infile);

    infile >> header;
    if (header != "#Background") {
        cerr << "Couldn't load background!";
        cerr << " May have improperly loaded foreground.\n";
    }

    loadLayer(MapLayer::BACKGROUND, infile);

    /* Load biome informaion. */
    infile >> header;
    if (header != "#Biomes") {
        cerr << "Couldn't load biome information! ";
        cerr << "May have improperly loaded background. \n";
    }

    /* The biome information has the same simple compression as the foreground
    and background. */
    int biomeInt;
    int count;
    int index = 0;
    while (index < biomesWide * biomesHigh) {
        infile >> count >> biomeInt;
        for (int i = 0; i < count; i++) {
            assert(index < biomesWide * biomesHigh);
            biomes[index].biome = (BiomeType)biomeInt;
            index++;
        }
    }

    /* Load the other data. */
    infile >> header;
    if (header != "#Other") {
        cerr << "Couldn't load tile information! ";
        cerr << "May have improperly loaded biomes. \n";
    }

    for (int i = 0; i < width * height; i++) {
        int spritePlace;
        infile >> spritePlace;
        tiles[i].foregroundSprite = (uint8_t)spritePlace;
        infile >> spritePlace;
        tiles[i].backgroundSprite = (uint8_t)spritePlace;
    }

    /* Iterate over the entire map. */
    Location fore;
    Location back;
    fore.layer = MapLayer::FOREGROUND;
    back.layer = MapLayer::BACKGROUND;
    for (int i = 0; i < width; i++) {
        fore.x = i;
        back.x = i;
        for (int j = 0; j < height; j++) {
            fore.y = j;
            back.y = j;
            /* Add the appropriate tiles to our list of tiles to update. */
            addToUpdate(fore);
            addToUpdate(back);
        }
    }
}

void Map::savePPM(MapLayer layer, std::string filename) {
    ofstream outfile(filename + ".ppm");
    /* Header saying we'll use ASCII, along with the height, width,
    and maximum value. */
    outfile << "P3\n" << width << " " << height << "\n255\n";
    for (int j = height - 1; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
            Tile *tile = getTile(i, j, layer);
            /* Output red, green, and blue values for each tile. */
            outfile << (int)(tile -> getColor().r) << " ";
            outfile << (int)(tile -> getColor().g) << " ";
            outfile << (int)(tile -> getColor().b) << " ";
        }
        outfile << "\n";
    } 
}


Light Map::getBiomeColor(BiomeInfo biome) const {
    Light color;
    switch(biome.biome) {
        case BiomeType::TUNDRA:
            color = Light(255, 255, 255, 0);
            break;
        case BiomeType::TAIGA:
            color = Light(128, 0xCC, 255, 0);
            break;
        case BiomeType::GRASSLAND:
            color = Light(58, 198, 88, 0);
            break;
        case BiomeType::WOODLAND:
            color = Light(87, 64, 36, 0);
            break;
        case BiomeType::JUNGLE:
            color = Light(25, 135, 23, 0);
            break;
        case BiomeType::SAVANNAH:
            color = Light(181, 51, 0, 0);
            break;
        case BiomeType::SCRUB:
            color = Light(246, 130, 53, 0);
            break;
        case BiomeType::DESERT:
            color = Light(215, 195, 115, 0);
            break;
        case BiomeType::TALLGRASS:
            color = Light(20, 126, 68, 0);
            break;
        default:
            color = Light(0, 0, 0, 0);
            break;
    }
    return color;
}

void Map::saveBiomePPM(std::string filename) {
    ofstream outfile(filename + "_biomes.ppm");
    /* Header saying we'll use ASCII, along with the height, width,
    and maximum value. */
    outfile << "P3\n" << width << " " << height << "\n255\n";
    for (int j = height - 1; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
            Light biomeColor = getBiomeColor(*getBiome(i, j));
            if (getTileType(i, j, MapLayer::FOREGROUND) == TileType::EMPTY) {
                biomeColor = biomeColor.times(0.5);
            }
            /* Output red, green, and blue values for each tile. */
            outfile << (int)(biomeColor.r) << " ";
            outfile << (int)(biomeColor.g) << " ";
            outfile << (int)(biomeColor.b) << " ";
        }
        outfile << "\n";
    } 
}

TileType Map::getTileType(int x, int y, MapLayer layer) const {
    if (!isOnMap(x, y)) {
        return TileType::EMPTY;
    }
    if (layer == MapLayer::FOREGROUND) {
        return findPointer(x, y) -> foreground;
    }
    else {
        assert(layer == MapLayer::BACKGROUND);
        return findPointer(x, y) -> background;
    }
}

void Map::setTile(int x, int y, MapLayer layer, TileType val) {
    assert(layer == MapLayer::FOREGROUND || layer == MapLayer::BACKGROUND
            || layer == MapLayer::NONE);

    bool wasSky = isSky(x, y);

    if (layer == MapLayer::FOREGROUND) {
        findPointer(x, y) -> foreground = val;
    }
    else if (layer == MapLayer::BACKGROUND) {
        findPointer(x, y) -> background = val;
    }
    else {
        /* We didn't change anything. */
        return;
    }

    /* If we made it this far we changed something, so the amount of light
    reaching nearby tiles may have changed. */
    updateNear(x, y);
    if ((wasSky && !getTile(val) -> getIsSky())
            || getForeground(x, y) -> getEmitted() != Light(0, 0, 0, 0)) {
        findPointer(x, y) -> lightRemoved = true;
    }
    if ((isSky(x, y) && !wasSky)
            || getTile(val) -> getEmitted() != Light(0, 0, 0, 0)) {
        findPointer(x, y) -> lightAdded = true;
    }
}

bool Map::placeTile(Location place, TileType type) {
    /* Can only place a tile if there isn't one there already. 
    Placing over water is allowed. */
    if (getTile(place) -> type != TileType::EMPTY
            && getTile(place) -> type != TileType::WATER) {
        return false;
    }

    /* Can only place a tile if one in the foreground or background 
    is next to it. */
    if (!isBesideTile(place.x, place.y, MapLayer::FOREGROUND)
            && !isBesideTile(place.x, place.y, MapLayer::BACKGROUND)) {
        return false;
    }

    /* Can only place certain types of tiles in the background. */
    if (place.layer == MapLayer::BACKGROUND 
            && !getTile(type) -> getCanBackground()) {
        return false;
    }

    setTile(place, type);
    chooseSprite(place.x, place.y);
    // TODO: change this if I add furniture

    return true;
}

void Map::update(vector<DroppedItem*> &items) {
    /* Make sure we're updating tiles that need to be updated. */
    set<Location>::iterator removeIter = toUpdate.begin();
    while (removeIter != toUpdate.end()) {
        if (!(getTile(*removeIter) -> canUpdate(*this, *removeIter))) {
            removeIter = toUpdate.erase(removeIter);
        }
        else {
            ++removeIter;
        }
    }

    /* Iterate over a copy of the list. */
    set<Location> newUpdate = toUpdate;
    set<Location>::iterator iter = newUpdate.begin();
    while (iter != newUpdate.end()) {
        Tile *tile = getTile(*iter);
        tile -> update(*this, *iter, items, tick);
        iter++;
    }

    /* Heal tiles that have been damaged for a while. */
    /* Tiles stay damaged for this many ticks, with about 20-40 ticks/sec. */
    const int healTime = 3000;
    /* Iterate while removing. */
    vector<TileHealth>::iterator healIter = damaged.begin();
    while (healIter != damaged.end()) {
        if (tick - healIter -> lastUpdated > healTime) {
            healIter = damaged.erase(healIter);
        }
        else {
            ++healIter;
        }
    }

    /* It's a new tick. */
    tick++;
}

bool Map::damage(Location place, int amount, vector<DroppedItem*> &items) {
    /* If there's no tile here, just return false. */
    if (getTile(place) -> type == TileType::EMPTY
        || getTile(place) -> type == TileType::WATER) {
        return false;
    }

    /* Index of the tilehealth, or -1 if it doesn't exist. */
    int index = -1;
    /* First check if that tile has already been damaged before. */
    for (unsigned int i = 0; i < damaged.size(); i++) {
        if (damaged[i].place == place) {
            damaged[i].health -= amount;
            damaged[i].lastUpdated = tick;
            index = i;
        }
    }

    /* If not, we add a new entry to the list. */
    if (index == -1) {
        damaged.emplace_back();
        damaged.back().place = place;
        damaged.back().health = getTile(place) -> getMaxHealth() - amount;
        damaged.back().lastUpdated = tick;
        index = damaged.size() - 1;
    }

    /* Now see if we need to destroy the tile. */
    if (destroy(damaged[index], items)) {
        /* If it was destroyed, removed it from the list. */
        damaged.erase(damaged.begin() + index);
    }

    return true;
}

void Map::kill(int x, int y, MapLayer layer, vector<DroppedItem*> &items) {
    // Drop itself as an item
    TileType type = getTileType(wrapX(x), y, layer);
    assert(type != TileType::WATER);
    if (type != TileType::EMPTY) {
        items.push_back(new DroppedItem ((ItemMaker::makeItem(
            ItemMaker::tileToItem(type), path)), 
            x * TILE_WIDTH, y * TILE_HEIGHT, width * TILE_WIDTH));
    }

    // Set the place it used to be to empty
    setTile(x, y, layer, TileType::EMPTY);
}

Location Map::getMapCoords(int x, int y, MapLayer layer) {
    assert(layer == MapLayer::FOREGROUND || layer == MapLayer::BACKGROUND
            || layer == MapLayer::NONE);
    
    /* Make sure x isn't negative. */
    x += getWidth() * getTileWidth();
    assert(x >= 0);

    /* Create a location to store the answer in. */
    Location place;
    place.layer = layer;

    /* Divide to get map coordinates. */
    place.x = x / getTileWidth();
    place.y = y / getTileHeight();

    /* And make sure they actually point to somewhere on the map. */
    place.x = wrapX(place.x);

    /* Double check our work. */
    assert(0 <= place.x);
    assert(place.x < getWidth());
    assert(0 <= place.y);
    assert(place.y < getHeight());

    return place;
}

void Map::moveTile(const Location &place, int x, int y, 
        vector<DroppedItem*> &items) {
    assert(place.x >= 0);
    assert(place.x < width);
    assert(place.y >=0);
    assert(place.y < height);
    assert(x != 0 || y != 0);
    int newX = wrapX(place.x + x);
    /* If it's just above the bottom, it vanishes. */
    if (!isOnMap(newX, place.y + y)) {
        setTile(place, TileType::EMPTY);
        return;
    }

    kill(newX, place.y + y, place.layer, items);
    TileType val = getTileType(place, 0, 0);
    Location spritePlace = getSprite(place);
    setSprite(newX, place.y + y, place.layer, spritePlace);
    setTile(newX, place.y + y, place.layer, val);
    setTile(place, TileType::EMPTY);
}

void Map::displaceTile(const Location &place, int x, int y) {
    assert(x != 0 || y != 0);
    int newX = wrapX(place.x + x);
    /* Check if the place below is actually on the map. */
    if (!isOnMap(newX, place.y + y)) {
        return;
    }

    TileType destination = getTileType(place, x, y);
    Location spritePlace = getSprite(newX, place.y + y, place.layer);
    setSprite(newX, place.y + y, place.layer, getSprite(place));
    setSprite(place, spritePlace);
    setTile(newX, place.y + y, place.layer, getTile(place) -> type);
    setTile(place, destination);
}

