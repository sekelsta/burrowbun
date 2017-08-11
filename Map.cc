#include <iostream>
#include <fstream> // To read and write files
#include <cassert>
#include <ctime> // To seed the random number generator
#include <cstdlib> // For randomness
#include <math.h> // Because pi
#include <tgmath.h> // for exponentiation
#include "Map.hh"
#include "Boulder.hh"

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

// Make a new Tile * or one of its child classes
Tile *Map::newTile(TileType val) {
    Tile *tile = NULL;
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

// Find a Tile object of type val. If it does not exist, create it. If 
// multiple exist, return the first one found.
Tile *Map::getTile(TileType val) {
    /* Return the tile if it exists. */
    if (pointers.size() > (unsigned int)val
            && pointers[(unsigned int)val] != NULL) {
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
        assert(pointers.back() == NULL);
    }
    /* Now stick in our tile. */
    pointers[(unsigned int)val] = tile;
 
    return tile;
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
    /* WrapX is called so it matches up with the tile on the other side
    of the map, wich it's next to when it wraps around. */
    if (getForeground(wrapX(x + 1), y) -> type == TileType::EMPTY) {
            col += 2;
    }
    if (y != 0
        && getForeground(x, y - 1) -> type == TileType::EMPTY) {
            col += 4;
    }
    if (getForeground(wrapX(x - 1), y) -> type == TileType::EMPTY) {
            col += 8;
    }
    findPointer(x, y) -> spritePlace.y = col;
    int row = rand() % getForeground(x, y) -> sprite.cols;
    findPointer(x, y) -> spritePlace.x = row;
}

/* Return true if there's a nonempty tile of the same layer at or next to 
this place. */
bool Map::isBesideTile(int x, int y, MapLayer layer) {
    /* Check at this place. */
    if (getTile(x, y, layer) -> type != TileType::EMPTY) {
        return true;
    }

    /* Check each tile next to it. */
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            // Don't check the status of tiles off the edge of the map, 
            // or if it's part of a diagonal line through the center tile
            if (isOnMap(x + i, y + j) && i != j && i != -1 * j
                    && getTile(x+i, y+j, layer) -> type != TileType::EMPTY) {
                return true;
            }
        }
    }

    return false;

}

/* Return true if neither the foreground nor background is opaque. Accept
out-of-bounds x coordintes and loop them so they are in bounds. */
inline bool Map::isSky(int x, int y) {
    x = wrapX(x);
    return (getForeground(x, y) -> getOpacity() == 0
        && getBackground(x, y) -> getOpacity() == 0);
}

/* Return the square of the distance between i, j and x, y. */
int Map::distance(int i, int j, int x, int y) {
    return (((i - x) * (i - x)) + ((j - y) * (j - y)));
}

/* Compute the square of the distance to the nearest sky tile that is a source 
   of light (doesn't have an opaque foreground or background). If the distance 
   is more than maxDist, return maxDist. */
int Map::skyDistance(int x, int y, int maxDist) {
    if (isSky(x, y) || getForeground(x, y) -> getOpacity() == 0) {
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

/* Calculate how well-lit the tile is. */
void Map::setLight(int x, int y) {
    // TODO: actually make this depend on the light source
    SpaceInfo *place = findPointer(x, y);
    place -> light.r = 0;
    place -> light.g = 0;
    place -> light.b = 0;
    int dist = skyDistance(x, y, 25);
    // make sure skyIntensity is between 0 and 255
    double lightIntensity = min(1.0, exp((1 - dist) / 8.0));
    place -> light.skyIntensity = 255 * max(0.0, lightIntensity);
    // And now the values are correct
    place -> isLightUpdated = true;
}

/* Return true if this is a plce on the map. */
bool Map::isOnMap(int x, int y) const {
    return (x >= 0 && y >= 0 && x < width && y < height);
}

/* Add a placeto the list of places to be updated, if the tile there will ever
need to be updated. */
void Map::addToUpdate(int x, int y, MapLayer layer) {
    /* Ignore it if it isn't a type of tile that needs to be updated. */
    if (!getTile(x, y, layer) -> getNeedsUpdating()) {
        return;
    }

    /* Time to add it to the list. */
    Location place;
    place.x = x;
    place.y = y;
    place.layer = layer;
    toUpdate.insert(place);
}

/* Go through the list of tiles to update and remove the ones that don't 
need to be updated anymore. */
void Map::trimUpdateList() {
    set<Location>::iterator iter = toUpdate.begin();
    while (iter != toUpdate.end()) {
        /* Remove if expired. */
        if (!getTile(*iter) -> getNeedsUpdating()) {
            iter = toUpdate.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

/* Tell all the tiles nearby to have the right sprite and the right amount of 
light, and recheck if they need to run their own update functions. */
void Map::updateNear(int x, int y) {
    // Update the sprites and tiles
    /* Value that takes into account x-wrapping of the map. */
    int newx;
    for (int i = -1; i < 2; i++) {
        newx = wrapX(x + i);
        for (int j = -1; j < 2; j++) {
            if (isOnMap(newx, y + j)) {
                /* Update the tiles. */
                addToUpdate(newx, y + j, MapLayer::FOREGROUND);
                addToUpdate(newx, y + j, MapLayer::BACKGROUND);
                /* Update the sprites. */
                chooseSprite(newx, y + j);
            }
        }
    }

    // Update the lighting
    // The range to update lighting, in each direction
    int range = 8;
    for (int i = -1 * range; i < range + 1; i++) {
        newx = wrapX(x + i);
        for (int j = -1 * range; j < range + 1; j++) {
            if (isOnMap(newx, y + j)) {
                findPointer(newx, y + j) -> isLightUpdated = false;
            }
        }
    }
}

// Public methods

// Constructor, based on a world file that exists
Map::Map(string filename, int tileWidth, int tileHeight) 
        : TILE_WIDTH(tileWidth), TILE_HEIGHT(tileHeight) {
    /* It's the 0th tick. */
    tick = 0;
    ifstream infile(filename);

    /* Create a tile object for each type. */
    for (int i = 0; i <= (int)TileType::LAST_TILE; i++) {
        getTile((TileType)i);
    }

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

    // Read the seed
    infile >> seed;

    // Read the map
    int index = 0;
    int count, tile;
    TileType foregroundTile;
    while (index < height * width) {
        infile >> count >> tile;
        foregroundTile = (TileType)tile;
        for (int i = 0; i < count; i++) {
            assert(index < width * height);
            tiles[index].foreground = foregroundTile;
            tiles[index].background = TileType::EMPTY; // TODO: load background
            index++;
        }
    }
    assert(getForeground(0, 0) -> type == tiles[0].foreground);

    /* Iterate over the entire map. */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            /* Set special sprites for all the tiles that need them. */
            if (getForeground(i, j) -> type != TileType::EMPTY) {
                chooseSprite(i, j);
            }
            // Tell the spaces they should figure out how well-lit they are 
            // before they get rendered
            findPointer(i, j) -> isLightUpdated = false;
            /* Add the appropriate tiles to our list of tiles to update. */
            addToUpdate(i, j, MapLayer::FOREGROUND);
            addToUpdate(i, j, MapLayer::BACKGROUND);
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

// Return the height, in pixels, of each tile
int Map::getTileHeight() const {
    return TILE_HEIGHT;
}

// Return the height, in pixels, of each tile
int Map::getTileWidth() const {
    return TILE_WIDTH;
}

// Return the default spawn point
Location Map::getSpawn() const {
    return spawn;
}

// Return the part of the spritesheet that should be used
Location Map::getSpritePlace(int x, int y) const {
    return findPointer(x, y) -> spritePlace;
}

// Return lighting
Light Map::getLight(int x, int y) {
    // Set the light to the correct value, if necessary
    if (!findPointer(x, y) -> isLightUpdated) {
        setLight(x, y);
    }

    Light light;
    // Combine the value from blocks with the value from the sky, taking into
    // account that the color of light the sky makes
    light.useSky(findPointer(x, y) -> light, getSkyColor(x, y));
    return light;
}

// Returns the color the sun or moon is shining
Light Map::getSkyColor(int x, int y) const {
    Light light;
    light.r = 255;
    light.g = 255;
    light.b = 255;
    light.skyIntensity = 255;
    return light;
}

/* Returns the tile at x, y, layer. */
Tile *Map::getTile(Location place) {
    return getTile(place.x, place.y, place.layer);
}

/* Returns the tile at x, y, layer. */
Tile *Map::getTile(int x, int y, MapLayer layer) {
    if (layer == MapLayer::FOREGROUND) {
        return getTile(findPointer(x, y) -> foreground);
    }
    else if (layer == MapLayer::BACKGROUND) {
        return getTile(findPointer(x, y) -> background);
    }
    else {
        return NULL;
    }
}

// Returns the foreground tile at x, y
// 0, 0 is the bottom right
Tile *Map::getForeground(int x, int y) {
    return getTile(findPointer(x, y) -> foreground);
}

// Returns the background tile at x, y
// 0, 0 is the bottom right
Tile *Map::getBackground(int x, int y) {
    return getTile(findPointer(x, y) -> background);
}

/* Get the type of the tile at place.x + x, place.y + y, place.layer. */
TileType Map::getTileType(const Location &place, int x, int y) {
    int newX = wrapX(place.x + x);
    if (place.layer == MapLayer::FOREGROUND) {
        return findPointer(newX, place.y + y) -> foreground;
    }
    else {
        assert(place.layer == MapLayer::BACKGROUND);
        return findPointer(newX, place.y + y) -> background;
    }
}

/* Sets the tile at x, y, layer equal to val. */
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

/* Sets the tile at x, y, layer equal to val. */
void Map::setTile(const Location &place, TileType val) {
    setTile(place.x, place.y, place.layer, val);
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
    // TODO: change this when I add furniture

    return true;
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
void Map::save(const string &filename) {
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

/* Update the map. */
void Map::update(vector<movable::Movable*> &movables) {
    // TODO
    /* Update tiles. */
    /* Iterate over a copy of the list. */
    set<Location> newUpdate = toUpdate;
    set<Location>::iterator iter = newUpdate.begin();
    while (iter != newUpdate.end()) {
        Tile *tile = getTile(*iter);
        tile -> update(*this, *iter, movables, tick);
        iter++;
    }
    /* And get rid of the ones that don't need to be updated anymore. */
    trimUpdateList();

    /*
    for (unsigned int i = 0; i < toUpdate.size(); i++) {
        Tile *tile = getTile(toUpdate[i]);
        tile -> update(*this, toUpdate[i], movables, tick);
    }*/

    /* Heal tiles that have been damaged for a while. */
    /* Tiles stay damaged for this many ticks, with about 20-40 ticks/sec. */
    const int healTime = 300;
    /* Iterate while removing. */
    vector<TileHealth>::iterator it = damaged.begin();
    while (it != damaged.end()) {
        if (tick - it -> lastUpdated > healTime) {
            it = damaged.erase(it);
        }
        else {
            ++it;
        }
    }

    /* It's a new tick. */
    tick++;
}

/* Damage the tile. Return false if there was no tile there to damage. */
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

/* Destroy a tile if it has no health left. */
bool Map::destroy(const TileHealth &health) {
    if (health.health <= 0) {
        kill(health.place);
    }

    return health.health <= 0;
}

/* Destroy a tile and (TODO) drop itself as an item. */
void Map::kill(int x, int y, MapLayer layer) {
    setTile(x, y, layer, TileType::EMPTY);
}
void Map::kill(const Location &place) {
    kill(place.x, place.y, place.layer);
}

/* Take an invalid x location and add or subtract the width until
0 <= x < width. */
int Map::wrapX(int x) {
    /* Fix if x is too large. */
    x %= getWidth();

    /* Fix if x is too small. */
    while (x < 0) {
        x += getWidth();
    }

    return x;
}

/* Take in world coordinates and a layer and convert to a location in 
map coordinates. */
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

/* Move a tile one down. If it was just above the bottom, it ceases to exist. */
void Map::moveDown(const Location &place) {
    /* If it's just above the bottom, it vanishes. */
    if (!isOnMap(place.x, place.y - 1)) {
        setTile(place, TileType::EMPTY);
        return;
    }

    /* Otherwise, kill the tile below and move down. */
    kill(place.x, place.y - 1, place.layer);
    setTile(place.x, place.y - 1, place.layer, getTile(place) -> type);
    setTile(place, TileType::EMPTY);
}

/* Switch this tile with the one below it. If there is no tile below it, it 
does nothing. */
void Map::displaceDown(const Location &place) {
    /* Check if the place below is actually on the map. */
    if (!isOnMap(place.x, place.y - 1)) {
        return;
    }

    /* And displace. */
    TileType temp = getTile(place) -> type;
    setTile(place, getTile(place.x, place.y - 1, place.layer) -> type);
    setTile(place.x, place.y - 1, place.layer, temp);
}

/* Move a tile dist in the +x direction. If there's a tile in the way,
it will be destroyed. Any tiles along the path are ignored. */
void Map::moveSideways(const Location &place, int dist) {
    assert(dist != 0);
    int newX = wrapX(place.x + dist);
    kill(newX, place.y, place.layer);
    TileType val = getTileType(place, 0, 0);
    setTile(newX, place.y, place.layer, val);
    setTile(place, TileType::EMPTY);
}

/* Move a tile dist in the +x direction. If there's a tile there, they
switch places. */
void Map::displaceSideways(const Location &place, int dist) {
    assert(dist != 0);
    int newX = wrapX(place.x + dist);
    TileType destination = getTileType(place, dist, 0);
    setTile(newX, place.y, place.layer, getTile(place) -> type);
    setTile(place, destination);
}

