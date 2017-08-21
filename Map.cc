#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime> // To seed the random number generator
#include <cstdlib> // For randomness
#include <math.h> // Because pi
#include <tgmath.h> // for exponentiation
#include "Map.hh"
#include "Boulder.hh"
#include "version.hh"

using namespace std;

Tile *Map::newTile(TileType val) {
    Tile *tile = nullptr;
    /* If it's a boulder, make a boulder. */
    if ((unsigned int)TileType::FIRST_BOULDER <= (unsigned int)val
            && (unsigned int)val <= (unsigned int)TileType::LAST_BOULDER) {
        tile = new Boulder(val);
    }
    /* Otherwise it's just a plain tile. */
    else {
        tile = new Tile(val);
        assert((unsigned int)TileType::FIRST_TILE <= (unsigned int)val);
        assert((unsigned int)val <= (unsigned int)TileType::LAST_PURE_TILE);
    }

    /* Set values that aren't set by the constructor. */
    tile -> sprite.width = TILE_WIDTH;
    tile -> sprite.height = TILE_HEIGHT;
    return tile;
}

Tile *Map::getTile(TileType val) {
    /* Return the tile if it exists. */
    if (pointers.size() > (unsigned int)val
            && pointers[(unsigned int)val] != nullptr) {
        return pointers[(unsigned int)val];
    }
    /* Otherwise make a new tile (or appropriate tile subclass) and add it to 
    the list of pointers. */
    Tile *tile = newTile(val);

    /* If the vector isn't big enough to hold this tile with its value as the
    index, resize it so this is so annd fill the new part with zeros. */
    if (pointers.size() <= (unsigned int)val) {
        pointers.resize((unsigned int)val + 1);
        /* Make sure it did get filled with zeros. */
        assert(pointers.back() == nullptr);
    }
    /* Now stick in our tile. */
    pointers[(unsigned int)val] = tile;
 
    return tile;
}

void Map::chooseSprite(int x, int y) {
    Location place;
    place.x = x;
    place.y = y;
    place.layer = MapLayer::FOREGROUND;
    uint8_t spritePlace = getTile(place) -> getSpritePlace(*this, place);
    findPointer(x, y) -> foregroundSprite = spritePlace;
    place.layer = MapLayer::BACKGROUND;
    spritePlace = getTile(place) -> getSpritePlace(*this, place);
    findPointer(x, y) -> backgroundSprite = spritePlace;
}

bool Map::isBesideTile(int x, int y, MapLayer layer) {
    /* Check at this place. */
    if (getTile(x, y, layer) -> type != TileType::EMPTY) {
        return true;
    }

    /* Check each tile next to it. */
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            /* Don't check the status of tiles off the edge of the map, 
            or if it's part of a diagonal line through the center tile. */
            if (isOnMap(x + i, y + j) && i != j && i != -1 * j
                    && getTile(x+i, y+j, layer) -> type != TileType::EMPTY) {
                return true;
            }
        }
    }

    return false;

}

inline bool Map::isSky(int x, int y) {
    x = wrapX(x);
    return (getForeground(x, y) -> getOpacity() == 0
        && getBackground(x, y) -> getOpacity() == 0);
}

int Map::skyDistance(int x, int y, int maxDist) {
    if (isSky(x, y)) {
        return 0;
    }
    int smallest = distance(0, 0, 0, maxDist);
    /* It's okay if i is negative. */
    for (int i = x + 1 - maxDist; i < maxDist + x; i++) {
        for (int j = max(0, y + 1 - maxDist);
                j < maxDist + y && j < height; j++) {
            if (isSky(i, j)) {
                smallest = min(smallest, distance(i, j, x, y));
            }
        }
    }
    return smallest;
}

void Map::setLight(int x, int y) {
    // TODO: actually make this depend on the light source
    SpaceInfo *place = findPointer(x, y);
    place -> light.r = 0;
    place -> light.g = 0;
    place -> light.b = 0;
    int dist = skyDistance(x, y, 25);
    /* Make sure skyIntensity is between 0 and 255. */
    double lightIntensity = min(1.0, exp((1 - dist) / 8.0));
    place -> light.skyIntensity = 255 * max(0.0, lightIntensity);

    place -> isLightUpdated = true;
}

void Map::updateNear(int x, int y) {
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

    /* Update the lighting. */
    int range = 8;
    for (int i = -1 * range; i < range + 1; i++) {
        int newx = wrapX(x + i);
        for (int j = -1 * range; j < range + 1; j++) {
            if (isOnMap(newx, y + j)) {
                findPointer(newx, y + j) -> isLightUpdated = false;
            }
        }
    }
}


int Map::bordering(const Location &place) const {
    int col = 0;
    if (place.y != height - 1 
            && getTileType(place, 0, 1) == TileType::EMPTY) {
        col += 1;
    }
    /* WrapX is called so it matches up with the tile on the other side
    of the map, wich it's next to when it wraps around. */
    if (getTileType(place, 1, 0) == TileType::EMPTY) {
        col += 2;
    }
    if (place.y != 0 && getTileType(place, 0, -1) == TileType::EMPTY) {
        col += 4;
    }
    if (getTileType(place, -1, 0) == TileType::EMPTY) {
        col += 8;
    }
    return col;
}

void Map::saveLayer(MapLayer layer, ofstream &outfile) const {
    // For keeping track of a lot of the same tile in a row
    int count = 0;
    TileType current, last;

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

    /* All the other data. */
    outfile << "\n#Other\n";
    for (int i = 0; i < width * height; i++) {
        outfile << tiles[i].foregroundSprite << " ";
        outfile << tiles[i].backgroundSprite << " ";
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
Map::Map(string filename, int tileWidth, int tileHeight) : 
        TILE_WIDTH(tileWidth), TILE_HEIGHT(tileHeight) {
    /* It's the 0th tick. */
    tick = 0;
    ifstream infile(filename);

    /* Create a tile object for each type. */
    for (int i = 0; i <= (int)TileType::LAST_TILE; i++) {
        getTile((TileType)i);
    }

    /* Check that the file could be opened. */
    // TODO: use exceptions like a proper person
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
        cerr << MINOR << "." << PATCH << "." << "The save format may";
        cerr << "have changed.\n";
    }

    /* Read in the things. */
    infile >> width >> height;
    tiles = new SpaceInfo[width * height];
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

    /* Load the other data. */
    infile >> header;
    if (header != "#Other") {
        cerr << "Couldn't load tile information! ";
        cerr << "May have improperly loaded background. \n";
    }

    for (int i = 0; i < width * height; i++) {
        infile >> tiles[i].foregroundSprite;
        infile >> tiles[i].backgroundSprite;
    }

    /* Iterate over the entire map. */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            /* Add the appropriate tiles to our list of tiles to update. */
            addToUpdate(i, j, MapLayer::FOREGROUND);
            addToUpdate(i, j, MapLayer::BACKGROUND);
        }
    }
}

void Map::savePPM(MapLayer layer, std::string filename) {
    ofstream outfile(filename);
    /* Header saying we'll use ASCII, along with the height, width,
    and maximum value. */
    outfile << "P3\n" << width << " " << height << "\n255\n";
    for (int j = height - 1; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
            Tile *tile = getTile(i, j, layer);
            /* Output red, green, and blue values for each tile. */
            outfile << (int)(tile -> color.r) << " ";
            outfile << (int)(tile -> color.g) << " ";
            outfile << (int)(tile -> color.b) << " ";
        }
        outfile << "\n";
    } 
}

/* Return the lighting of a tile. */
Light Map::getLight(int x, int y) {
    /* Set the light to the correct value, if necessary. */
    if (!findPointer(x, y) -> isLightUpdated) {
        setLight(x, y);
    }

    Light light;
    /* Combine the value from blocks with the value from the sky, taking into
    account that the color of light the sky makes. */
    light.useSky(findPointer(x, y) -> light, getSkyColor(x, y));
    return light;
}

/* Returns the color the sun or moon is shining. */
Light Map::getSkyColor(int x, int y) const {
    Light light;
    light.r = 255;
    light.g = 255;
    light.b = 255;
    light.skyIntensity = 255;
    return light;
}

TileType Map::getTileType(const Location &place, int x, int y) const {
    int newX = wrapX(place.x + x);
    if (!isOnMap(newX, place.y + y)) {
        return TileType::EMPTY;
    }
    if (place.layer == MapLayer::FOREGROUND) {
        return findPointer(newX, place.y + y) -> foreground;
    }
    else {
        assert(place.layer == MapLayer::BACKGROUND);
        return findPointer(newX, place.y + y) -> background;
    }
}

void Map::setTile(int x, int y, MapLayer layer, TileType val) {
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
}

bool Map::placeTile(Location place, TileType type) {
    /* Can only place a tile if there isn't one there already. */
    if (getTile(place) -> type != TileType::EMPTY) {
        return false;
    }

    /* Can only place a tile if one in the foreground or background 
    is next to it. */
    if (!isBesideTile(place.x, place.y, MapLayer::FOREGROUND)
            && !isBesideTile(place.x, place.y, MapLayer::BACKGROUND)) {
        return false;
    }

    setTile(place, type);
    chooseSprite(place.x, place.y);
    // TODO: change this when I add furniture

    return true;
}

vector<Tile *> Map::getPointers() const {
    return pointers;
}

// TODO: rethink
vector<Tile *> &Map::getPointersRef() {
    return pointers;
}

void Map::update(vector<movable::Movable*> &movables) {
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
        tile -> update(*this, *iter, movables, tick);
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

bool Map::damage(Location place, int amount) {
    /* If there's no tile here, just return false. */
    if (getTile(place) -> type == TileType::EMPTY) {
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
    if (destroy(damaged[index])) {
        /* If it was destroyed, removed it from the list. */
        damaged.erase(damaged.begin() + index);
    }

    return true;
}

bool Map::destroy(const TileHealth &health) {
    if (health.health <= 0) {
        kill(health.place);
    }

    return health.health <= 0;
}

void Map::kill(int x, int y, MapLayer layer) {
    // TODO: drop itself as an item
    setTile(x, y, layer, TileType::EMPTY);
}
void Map::kill(const Location &place) {
    kill(place.x, place.y, place.layer);
}

Location Map::getMapCoords(int x, int y, MapLayer layer) {
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

void Map::moveTile(const Location &place, int x, int y) {
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

    kill(newX, place.y + y, place.layer);
    TileType val = getTileType(place, 0, 0);
    uint8_t spritePlace = getSprite(place);
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
    uint8_t spritePlace = getSprite(newX, place.y + y, place.layer);
    setSprite(newX, place.y + y, place.layer, getSprite(place));
    setSprite(place, spritePlace);
    setTile(newX, place.y + y, place.layer, getTile(place) -> type);
    setTile(place, destination);
}

