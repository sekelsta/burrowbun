#ifndef MAP_HH
#define MAP_HH

#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include "Tile.hh"
#include "MapHelpers.hh"

#define MAX_OPACITY 64

class DroppedItem;

/* In the biome information stored, each piee refers to a square this size of
tiles. */
#define BIOME_SIZE 32

/* A class for a map. Holds an array of SpaceInfos, which store the foreground
and background tiles, among other things. */
class Map {
    /* Mapgen is basically an extra-fancy constructor. */
    friend class Mapgen;

    const int TILE_WIDTH;
    const int TILE_HEIGHT;

    /* The seed it was created with. */
    int seed;

    /* How many ticks since the map was loaded. */
    unsigned int tick;

    /* The array to hold the map info. This is a 2d array squished into 1d. */
    SpaceInfo *tiles;

    /* The array to hold the biome info. */
    std::vector<BiomeInfo> biomes;

    /* A list of Tiles. They contain memory that must be manually garbage
    collected because of the SDL textures. */
    std::vector<Tile *> pointers;

    /* The height and width of the map, in number of tiles. */
    int height, width;

    /* The height and width of the biome array. */
    int biomesHigh, biomesWide;

    /* Default spawn point. It may be possible for players to set their own
    spawn points later. */
    Location spawn;

    /* The tiles whose update function should be called. */
    std::set<Location> toUpdate;

    /* Tiles that have been damaged. */
    std::vector<TileHealth> damaged;

    /* Table of pre-calculated exponentials. */
    std::vector<double> exps;

    /* Return a pointer to the SpaceInfo* at x, y. */
    inline SpaceInfo *findPointer(int x, int y) const {
        x = wrapX(x);
        assert (0 <= y);
        assert (y < height);
        return tiles + (y * width + x);
    }

    /* Make a Tile object (or one of its subclasses), add it to the list of 
    pointers, and return a pointer to it. */
    Tile *newTile(TileType val);

    /* Find a Tile object of type val. If it does not exist, create it. If
    multiple exist, return the first one. */
    inline Tile *getTile(TileType val) const {
        /* Return the tile if it exists. */
        assert(pointers[(unsigned int)val] != nullptr);
        return pointers[(unsigned int)val];
    }

    /* Call chooseSprite on every tile on the map. */
    void randomizeSprites();

    /* Pick the sprite to use for a tile based on the ones next to it. */
    void chooseSprite(int x, int y);

    /* Return true if there's a non-empty tile of the same maplayer here or 
    next to here. */
    bool isBesideTile(int x, int y, MapLayer layer);

    /* Return true if neither the foreground nor background are opaque. 
    Accept out-of-bounds x coordinates and loop them so they are in bounds. */
    bool isSky(int x, int y);

    /* Spread out the light from a source at x, y. */
    void effectLight(int x, int y, const Light &light, 
        std::vector<std::vector<int>> &current);

    /* Add an array for a light to the whole map. */
    void addLight(int x, int y, std::vector<std::vector<int>> &current,
        const Light &l);

public:
    /* Calculate how well-lit the tiles on screen are and set their light 
    levels. */
    void setLight(int xstart, int ystart, int xstop, int ystop);

private:
    /* Set the tiles around a place to show the right sprite and have the
    right amount of light, and recheck if they need to run their own update
    functions. */
    void updateNear(int x, int y);

    /* Private setter function since only friend class Mapgen should be using
    it. */
    inline void setHeight(int newHeight) {
        height = newHeight;
        biomesHigh = height / BIOME_SIZE + 1;
    }
    inline void setWidth(int newWidth) {
        width = newWidth;
        biomesWide = width / BIOME_SIZE + 1;
    }

    /* Get the biomeInfo of an x and y. */
    inline BiomeInfo *getBiome(int x, int y) {
        assert(0 <= x);
        assert(0 <= y);
        assert(x < biomesWide * BIOME_SIZE);
        assert(y < biomesHigh * BIOME_SIZE);
        int index = biomesWide * (y / BIOME_SIZE) + (x / BIOME_SIZE); 
        return &biomes[index];
    }

    /* Set the biomeInfo at x, y of the biomes vector (which is different than
    the tiles array by a factor of BIOME_SIZE) to a different biomeInfo. This 
    uses BIOME_SIZE spacing because any function calling this shouldn't really
    be setting the biome for a single x, y coordinate on the map, as that would
    also set all nearby tiles. */
    inline void setBiome(int x, int y, const BiomeInfo &info) {
        assert(0 <= x);
        assert(0 <= y);
        assert(x < biomesWide);
        assert(y < biomesHigh);
        int index = biomesWide * y + x;
        biomes[index] = info;
    }

    public:
    /* Add a place to the list of places to be updated, if the tile there
    will need to be updated. */
    inline void addToUpdate(const Location &place) {
        assert(0 <= place.x);
        assert(place.x < width);
        assert(0 <= place.y);
        assert(place.y < height);
        /* Ignore it if it won't need to be updated. */
        if (getTile(place) -> canUpdate(*this, place)) {
            toUpdate.insert(place);
        }
    }

    inline void addToUpdate(int x, int y, MapLayer layer) {
        Location place;
        place.x = x;
        place.y = y;
        place.layer = layer;
        addToUpdate(place);
    }

    inline void removeFromUpdate(const Location &place) {
        toUpdate.erase(place);
    }

    inline void removeFromUpdate(int x, int y, MapLayer layer) {
        Location place;
        place.x = x;
        place.y = y;
        place.layer = layer;
        removeFromUpdate(place);
    }

    inline bool updateContains(const Location &place) const {
        return toUpdate.count(place);
    }

    /* Calculates the coefficent for light when the opacity is n. */
    inline double getExpLight(int n) {
        if (n >= MAX_OPACITY) {
            return 0;
        }
        if (exps[n] == 0) {
            double coef = exp(-1 * n * n / (MAX_OPACITY * MAX_OPACITY / 4.0));
            exps[n] = std::min(1.0, std::max(coef, 0.0));
        }
        return exps[n];
    }

    /* Return a number from 0-15 depending on which tiles border this one. 
    (In fact, in binary it returns the number you get if you start at the left
    side and go counterclockwise around, reading an empty tile as a 0. )*/
    int bordering(const Location &place);

    /* Return true if this is a place that exists on the map. */
    inline bool isOnMap(int x, int y) const {
        return (x >= 0 && y >= 0 && x < width && y < height);
    }

    /* Save the foreground or background layer to a file. */
    void saveLayer(MapLayer layer, std::ofstream &outfile) const;

    /* Save the map to a file. */
    void save(std::string filename) const;

    /* Read the foreground or background layer in from the savefile. */
    void loadLayer(MapLayer layer, std::ifstream &infile);

    /* Constructor, from a savefile. */
    Map(std::string filename, int tileWidth, int tileHeight);

    /* Save the specified layer to a PPM file. */
    void savePPM(MapLayer layer, std::string filename);

    /* Return a color representing that biome. */
    Light getBiomeColor(BiomeInfo biome) const;

    /* Save a picture showing where all the biomes are. */
    void saveBiomePPM(std::string filename);
private:
    // Constructor. Resulting map cannot be played but can be saved.
    inline Map() : TILE_WIDTH(1), TILE_HEIGHT(1) {
        tiles = nullptr;

        /* Create a tile object for each type. */
        for (int i = 0; i <= (int)TileType::LAST_TILE; i++) {
            newTile((TileType)i);
        }
    }

public:
    /* Destructor */
    inline ~Map() {
        /* Delete the map. */
        delete[] tiles;
        /* Delete each tile object. */
        while (pointers.empty() == false) {
            delete pointers.back();
            pointers.pop_back();
        }
    }

    /* Return the height of the map, in number of tiles. */
    inline int getHeight() const {
        return height;
    }

    /* Return the width of the map, in number of tiles. */
    inline int getWidth() const {
        return width;
    }

    /* Return the height, in pixels, of each tile. */
    inline int getTileHeight() const {
        return TILE_HEIGHT;
    }

    /* Return the width, in pixels, of each tile. */
    inline int getTileWidth() const {
        return TILE_WIDTH;
    }

    /* Return the default spawn point. */
    inline Location getSpawn() const {
        return spawn;
    }

    /* Return which part of the spritesheet should be used. */
    inline uint8_t getForegroundSprite(int x, int y) const {
        return findPointer(x, y) -> foregroundSprite;
    }

    inline uint8_t getBackgroundSprite(int x, int y) const {
        return findPointer(x, y) -> backgroundSprite;
    }

    inline Location getSprite(int x, int y, MapLayer layer) const {
        uint8_t sprite;
        if (layer == MapLayer::FOREGROUND) {
            sprite = getForegroundSprite(x, y);
        }
        else {
            assert(layer == MapLayer::BACKGROUND);
            sprite = getBackgroundSprite(x, y);
        }

        Location answer;
        SpaceInfo::fromSpritePlace(answer, sprite);
        return answer;
    }

    inline Location getSprite(const Location &place) const {
        return getSprite(place.x, place.y, place.layer);
    }

    /* Set which part of the spritesheet should be used. */
    inline void setSprite(int x, int y, MapLayer layer, Location newSprite) {
        uint8_t toset = SpaceInfo::toSpritePlace(newSprite);
        if (layer == MapLayer::FOREGROUND) {
            findPointer(x, y) -> foregroundSprite = toset;
        }
        else {
            assert(layer == MapLayer::BACKGROUND);
            findPointer(x, y) -> backgroundSprite = toset;
        }
    }

    inline void setSprite(const Location &place, Location newSprite) {
        setSprite(place.x, place.y, place.layer, newSprite);
    }  

    /* Return the lighting of a tile. */
    inline Light getLight(int x, int y) {
        /* Combine the value from blocks with the value from the sky, taking into
        account that the color of light the sky makes. */
        return findPointer(x, y) -> light.useSky(getSkyLight());
    }

    /* Return the color the sun / moon is shining. */
    inline Light getSkyLight() const {
        return {255, 255, 255, 255};
    }

    /* Return the color the sky tiles should be rendered. */
    inline Light getSkyColor() const {
        return {0x00, 0x99, 0xFF, 0xFF};
    }

    /* Return the pointer the the tile at this location. */
    inline Tile *getTile(int x, int y, MapLayer layer) {
        return getTile(getTileType(wrapX(x), y, layer));
    }

    inline Tile *getTile(Location place) {
        return getTile(place.x, place.y, place.layer);
    }

    /* Returns the foreground tile pointer at x, y.
    0, 0 is the bottom right. */
    inline Tile *getForeground(int x, int y) {
        return getTile(findPointer(x, y) -> foreground);
    }

    /* Returns the background tile pointer at x, y.
    0, 0 is the bottom right. */
    inline Tile *getBackground(int x, int y) {
        return getTile(findPointer(x, y) -> background);
    }

    /* Get the type of the tile at x, y, layer. If it isn't on the map,
    return TileType::EMPTY. */
    TileType getTileType(int x, int y, MapLayer layer) const;

    /* Sets the tiletype very fast (does not update the sprites of the tiles
    around it). */
    inline void setTileType(int x, int y, MapLayer layer, TileType type) {
        if (layer == MapLayer::FOREGROUND) {
            findPointer(x, y) -> foreground = type;
        }
        else {
            assert(layer == MapLayer::BACKGROUND);
            findPointer(x, y) -> background = type;
        }
    }

    /* Get the type of the tile at place.x + x, place.y + y, place.layer. 
    If the tile isn't on the map, return TileType::EMPTY. */
    inline TileType getTileType(const Location &place, int x, int y) const {
        int newX = wrapX(place.x + x);
        return getTileType(newX, place.y + y, place.layer);
    }

    /* Set the tile at x, y, layer equal to val. */
    inline void setTile(const Location &place, TileType val) {
        setTile(place.x, place.y, place.layer, val);
    }

    void setTile(int x, int y, MapLayer layer, TileType val);

    /* Place a tile in the correct layer. Return whether it was successful. */
    bool placeTile(Location place, TileType type);

    /* Update the map. */
    void update(std::vector<DroppedItem*> &items);

    /* Damage a tile (with a pickax or something). Return false if there
    was no tile to damage. */
    bool damage(Location place, int amount, std::vector<DroppedItem*> &items);

    /* Destroy a tile if it has no health. Return true if it was destroyed, or
    false if it still had health and lived. */
    inline bool destroy(const TileHealth &health, 
            std::vector<DroppedItem*> &items) {
        if (health.health <= 0) {
            kill(health.place, items);
        }

        return health.health <= 0;
    }

    /* Destroy a tile. */
    void kill(int x, int y, MapLayer layer, std::vector<DroppedItem*> &items);
    inline void kill(const Location &place, std::vector<DroppedItem*> &items) {
        kill(place.x, place.y, place.layer, items);
    }

    /* Take an invalid x location and add or subtract width until
    0 <= x < width. */
    inline int wrapX(int x) const {
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
    Location getMapCoords(int x, int y, MapLayer layer);

    /* Move a tile x in the +x direction and y in the +y directoin. If there's 
    a tile at the destination, it will be destroyed. */
    void moveTile(const Location &place, int x, int y,
        std::vector<DroppedItem*> &items);

    /* Move a tile x in the +x direction and y in the +y direction. If there's 
    a tile there, they switch places. */
    void displaceTile(const Location &place, int x, int y);
};

#endif
