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

    /* Generate a tiny world good for testing world generation. */
    void generateSmolTest();

    /* Get the value on a cylinder from a noise module. This squishes all x
    values into the unit circle without affecting y values, so scale 
    adjustments may be needed. */
    double getCylinderValue(int x, int y, noise::model::Cylinder&) const;

public:
    /* Take a reference to a newly created map, and fill it with stuff. */
    void generate(std::string filename, WorldType worldType);
};

#endif
