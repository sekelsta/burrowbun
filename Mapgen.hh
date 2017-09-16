#ifndef MAPGEN_HH
#define MAPGEN_HH

#include <vector>
#include <string>
#include <random> // For the fancy randomness like a normal distribution
#include <noise.h>
#include "Tile.hh"
#include "MapHelpers.hh"
#include "Map.hh"

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

    /* Generate a tiny world good for testing world generation. */
    void generateEarth();

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
public:
    /* Take a reference to a newly created map, and fill it with stuff. */
    void generate(std::string filename, WorldType worldType);
};

#endif
