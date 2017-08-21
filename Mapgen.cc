#include <iostream>
#include <fstream> // To read and write files
#include <cassert>
#include <ctime> // To seed the random number generator
#include <cstdlib> // For randomness
#include <cmath> // Because pi and exponentiation
#include <algorithm> // For max and min
#include <noise.h> // Finding this requires having libnoise installed and may
// require looking in non-standard places.
#include "Mapgen.hh"
#include "version.hh"

using namespace std;

void Mapgen::generateSmolTest() {
    /* Set height and width, and use them to make a tile array. */
    map.height = 50;
    map.width = 50;
    assert(map.tiles == nullptr);
    map.tiles = new SpaceInfo[map.width * map.height];

    /* Make the bottom solid. */
    for (int i = 0; i < map.width; i++) {
        for (int j = 0; j < 20; j++) {
            map.setTile(i, j, MapLayer::FOREGROUND, TileType::MUDSTONE);
        }
    }
}

void Mapgen::generate(std::string filename, WorldType worldType) {
    /* Seed the random number generators. */
    map.seed = time(NULL);
    srand(map.seed);
    generator.seed(map.seed);

    /* Run the appropriate function. */
    switch(worldType) {
        case WorldType::TEST : 
            break;
        case WorldType::SMOLTEST :
            generateSmolTest();
            break;
        case WorldType::EARTH :
            break;
        default :
            cerr << "Maybe I'll implement that later." << endl;
    }

    map.spawn.x = map.width / 2;
    /* I should be careful to make sure there are never cloud cities or 
    floating islands or whatever directly above the spawn point, so the
    player doesn't die of fall damage every time they respawn. */
    map.spawn.y = map.height / 2;
    map.save(filename);
    /* TODO: remove when done testing. */
    map.savePPM(MapLayer::FOREGROUND, filename + ".ppm");
}


