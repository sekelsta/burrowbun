#include <iostream>
#include <fstream> // To read and write files
#include <cassert>
#include <ctime> // To seed the random number generator
#include <cstdlib> // For randomness
#include <cmath> // Because pi and exponentiation
#include <algorithm> // For max and min
#include "Mapgen.hh"
#include "version.hh"

using namespace std;
using namespace noise;

void Mapgen::generateSmolTest() {
    /* Set height and width, and use them to make a tile array. */
    map.setHeight(512);
    map.setWidth(1024);
    map.biomes.resize(map.biomesWide * map.biomesHigh);
    assert(map.tiles == nullptr);
    map.tiles = new SpaceInfo[map.width * map.height];
    cylinderScale.SetXScale((map.width / 2.0) / M_PI);
    cylinderScale.SetZScale(cylinderScale.GetXScale());

    /* Make a Perlin noise module for temperature, humidity, and magicalness,
    for use in determining biome. */
    module::Perlin baseTemperature;
    baseTemperature.SetOctaveCount(3);
    baseTemperature.SetPersistence(0.3);
    baseTemperature.SetSeed(rand());
    module::ScalePoint scaledTemperature;
    scaledTemperature.SetScale(0.01);
    scaledTemperature.SetSourceModule(0, baseTemperature);
    // TODO: same for humidity, magicalness

    /* Just for testing, make a map based entirely on temperature. */
    for (int i = 0; i < map.width; i++) {
        for (int j = 0; j < map.height; j++) {
            double temp = getCylinderValue(i, j, scaledTemperature);
            temp++;
            temp = std::min(2.0, std::max(temp, 0.0));
            temp *= (int)TileType::LAST_TILE;
            temp /= 2;
            TileType tiletype = (TileType)temp;
            map.setTile(i, j, MapLayer::FOREGROUND, tiletype);
        }
    }

}

double Mapgen::getCylinderValue(int x, int y, const module::Module &values) {
    cylinderScale.SetSourceModule(0, values);
    return cylinder.GetValue(x * 360.0 / map.width, y);
}

void Mapgen::generate(std::string filename, WorldType worldType) {
    /* Seed the random number generators. */
    map.seed = time(NULL);
    srand(map.seed);
    generator.seed(map.seed);

    /* Set the cylinder to get it's values from the scaled module. Of course,
    the scaled module will need to get its values from somewhere, too. */
    cylinder.SetModule(cylinderScale);

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


