#ifndef MAPGEN_HH
#define MAPGEN_HH

#include <vector>
#include <string>
#include <random> // For the fancy randomness like a normal distribution
#include <noise.h>
#include "Tile.hh"
#include "MapHelpers.hh"
#include "Map.hh"
#include <mutex>
#include <algorithm> // For max and min

/* How far along world creation is. */
enum class CreateState {
    NONE,
    STUFF,
    NOT_STARTED,
    GENERATING_BIOMES,
    GENERATING_TERRAIN,
    ADDING_GLOWSTONE,
    FELSIC,
    GENERATING_OCEAN,
    ADDING_DIRT,
    SETTLING_WATER,
    SAVING,
    DONE
};

/* A class for generating a map. */
class Mapgen {
    /* Have a random number generator. */
    std::default_random_engine generator;

    /* The seed that was used to generate the world. */
    int seed;

    /* The map to generate. */
    Map map;

    /* A cylinder to make noise models seamless at the edge. */
    noise::model::Cylinder cylinder;

    /* A module to scale it so the cylinder one looks normal. */
    noise::module::ScalePoint cylinderScale;

    /* A 2D vector saying which percentiles map to which biomes. */
    std::vector<std::vector<int>> biomeData;

    /* Some values for map generation. */
    int baseHeight;
    int seaLevel;
    int seafloorLevel;
    int cavernHeight;
    int shoreline;
    int abyss;

    /* A vector of the highest solid block for each x value, not counting any
    sort of floating island. */
    std::vector<int> surfaces;

    /* Set the map size to x, y. */
    void setSize(int x, int y);

    /* Generate a complex world. */
    void generateEarth(CreateState *state, std::mutex *m);

    /* Generate a tiny world good for testing world generation. */
    void generateTest();

    /* Get the value on a cylinder from a noise module. This squishes all x
    values into the unit circle without affecting y values, so scale 
    adjustments may be needed. */
    double getCylinderValue(int x, int y, const noise::module::Module &values);

    /* Get the number that percentile of the results will be smaller than,
    out of the given number of samples. */
    static double getPercentile(double percentile, 
        noise::module::Module &values, int samples);

    /* Choose a biome given a temperature and a humidity. This will not choose
    any biomes dependent on anything other than temperature and humidity (sky,
    cloud forest, ocean). The percentile vectors should be 7 long and (for the
    biomes to all be about equally common) should be the number that 0.125 of 
    the results were less than, 0.25 were less than, 0.375, and so on in 
    eigths. This is a little ugly but far more efficient than recalculating 
    for every x and y. */
    BiomeType getBaseBiome(double temperature, double humidity, 
        std::vector<double> tempPercentiles, 
        std::vector<double> humidityPercentiles);

    /* Get a value for determining the ground level changes needed for an
    ocean. */
    double ocean(int x, int y);

    inline void fillVertical(int x, int low, int high, MapLayer layer, 
            TileType type) {
        for (int y = low; y < high; y++) {
            map.setTileType(x, y, layer, type);
        }
    }

    /* Choose how felsic or mafic all the rock should be. */
    void setFelsic();

    /* Put dirt, clay, and sand on the surface. */
    void putDirt();

    /* Get a value for determining where the level of the surface should be. */
   double getSurface(int x, int y, const noise::module::Module &surface,
            WorldType type);

    /* Get whether this spot should be a large tunnel. */
    inline bool isTunnel(int x, int y, const noise::module::Module &tunnels, 
            double surface, double tunnelLimit, double cavernLimit) {
        double tunnel = getCylinderValue(x, y, tunnels);
        double tunnelHeight = (y - cavernHeight) / 50.0 / 2.0;
        return tunnel > tunnelLimit
            && std::max(surface, tunnelHeight + surface / 2.0) - tunnel < cavernLimit;
    }

    /* Move a tile on a map from x1, y1 to x2, y2 without updating the tiles 
    around it. */
    void moveTileFast(int x1, int y1, int x2, int y2, MapLayer layer);

    /* Helper function for settleWater. Returns x - direction if there's no 
    place it can fall. */
    int findFall(int direction, int x, int y, MapLayer layer);

    /* Helper function for settleWater, moves a water tile downwards and maybe
    sideways. */
    void moveWater(int x, int y);

    /* Puts a layer filldepth thick of water at the top of the map. */
    void fillWater(int fillDepth);

    /* Takes a map, and makes all water on it flow as far as it can go. */
    void settleWater();

    /* Takes a map, and removes the top removeDepth layers of water
    not protected by an overhang. */
    void removeWater(int removeDepth);
public:
    Mapgen(std::string path);

    /* Take a reference to a newly created map, and fill it with stuff. */
    void generate(std::string filename, WorldType worldType, std::string path, 
        CreateState *state, std::mutex *m);
};

#endif
