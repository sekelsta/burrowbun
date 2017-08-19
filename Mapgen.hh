#ifndef MAPGEN_HH
#define MAPGEN_HH

#include <vector>
#include <string>
#include <random> // For the fancy randomness like a normal distribution
#include "Tile.hh"
#include "MapHelpers.hh"
#include "Map.hh"

/* A class for generating a map. */
class Mapgen {
    /* Have a random number generator. */
    std::default_random_engine generator;

    /* The seed that was used to generate the world. */
    int seed;

    /* Generate a tiny world good for testing world generation. */
    void generateSmolTest();

    /* The map to generate. */
    Map map;

public:
    /* Take a reference to a newly created map, and fill it with stuff. */
    void generate(std::string filename, WorldType worldType);
};

#endif
