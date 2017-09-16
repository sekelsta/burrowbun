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
using json = nlohmann::json;

void Mapgen::generateEarth() {
    /* Set height and width, and use them to make a tile array. */
    map.setHeight(2048);
    map.setWidth(4096);
    map.biomes.resize(map.biomesWide * map.biomesHigh);
    assert(map.tiles == nullptr);
    map.tiles = new SpaceInfo[map.width * map.height];
    cylinderScale.SetXScale((map.width / 2.0) / M_PI);
    cylinderScale.SetZScale(cylinderScale.GetXScale());

    /* Some constants to use in the perlin moise. */
    const int octaves = 2;
    const double persistence = 0.2;
    const double scale = 0.04;

    /* Make a Perlin noise module for temperature, humidity, and magicalness,
    for use in determining biome. */
    module::Perlin baseTemperature;
    baseTemperature.SetOctaveCount(octaves);
    baseTemperature.SetPersistence(persistence);
    baseTemperature.SetSeed(rand());
    module::ScalePoint scaledTemperature;
    scaledTemperature.SetScale(scale);
    scaledTemperature.SetSourceModule(0, baseTemperature);

    /* Same, but for humidity. */
    module::Perlin baseHumidity;
    baseHumidity.SetOctaveCount(octaves);
    baseHumidity.SetPersistence(persistence);
    baseTemperature.SetSeed(rand());
    module::ScalePoint scaledHumidity;
    scaledHumidity.SetScale(scale);
    scaledHumidity.SetSourceModule(0, baseHumidity);

    const int nsamples = 10000;
    vector<double> tempPercentiles;
    vector<double> humidityPercentiles;
    /* Seven items long because that's what getBaseBiome() requires. */
    for (int i = 0; i < 7; i++) {
        double percentile = (i + 1) / 8.0;
        tempPercentiles.push_back(getPercentile(percentile, scaledTemperature,
            nsamples));
        humidityPercentiles.push_back(getPercentile(percentile, scaledHumidity,
            nsamples));
    }

    /* Use a crude calculation to get some of the biomes (just for testing). */
    for (int i = 0; i < map.biomesWide; i++) {
        for (int j = 0; j < map.biomesHigh; j++) {
            double temperature = getCylinderValue(i, j, scaledTemperature);
            double humidity = getCylinderValue(i, j, scaledHumidity);
            BiomeInfo info;
            info.biome = getBaseBiome(temperature, humidity, tempPercentiles,
                humidityPercentiles);
            map.setBiome(i, j, info);
        }
    }

    /* Just for testing, base the foreground tile on biome. */
    for (int i = 0; i < map.width; i++) {
        for (int j = 0; j < map.height; j++) {
            TileType tileType = TileType::EMPTY;
            BiomeType biome = map.getBiome(i, j) -> biome;
            if (biome == BiomeType::DESERT) {
                tileType = TileType::SANDSTONE;
            }
            else if (biome == BiomeType::SCRUB) {
                tileType = TileType::GRANITE;
            }
            else if (biome == BiomeType::SAVANNAH) {
                tileType = TileType::RED_SANDSTONE;
            }
            else if (biome == BiomeType::JUNGLE) {
                tileType = TileType::BASALT;
            }
            else if (biome == BiomeType::WOODLAND) {
                tileType = TileType::PLATFORM;
            }
            else if (biome == BiomeType::GRASSLAND) {
                tileType = TileType::PERIDOTITE;
            }
            else if (biome == BiomeType::TAIGA) {
                tileType = TileType::ICE;
            }
            else if (biome == BiomeType::TUNDRA) {
                tileType = TileType::SNOW;
            }
            map.setTile(i, j, MapLayer::FOREGROUND, tileType);
        }
    }

}

double Mapgen::getCylinderValue(int x, int y, const module::Module &values) {
    cylinderScale.SetSourceModule(0, values);
    return cylinder.GetValue(x * 360.0 / map.width, y);
}

double Mapgen::getPercentile(double percentile, module::Module &values, 
        int samples) {
    /* Seed rand() with the module's seed so this always gives the same result
    for the same module. */
    vector<double> results;
    for (int i = 0; i < samples; i++) {
        double value = values.GetValue(rand(), rand(), rand());
        /* Find where in the vector to insert it. */
        int j;
        for (j = 0; j < i; j++) {
            if (value <= results[j]) {
                results.insert(results.begin() + j, value);
                break;
            }
        }
        /* If it wasn't added, it's the biggest number and should be added to
        the end. */
        if (j == i) {
            results.push_back(value);
        }
        assert((int)results.size() == i + 1);
    }

    int index = (int)(percentile * (double)samples);
    assert(index <= samples);
    assert(0 <= index);
    return results[index];
}

BiomeType Mapgen::getBaseBiome(double temperature, double humidity, 
        vector<double> tempPercentiles, vector<double> humidityPercentiles) {
    assert(tempPercentiles.size() == 7);
    assert(humidityPercentiles.size() == 7);

    /* Find which percentile it's in, and use our biomeData vector to figure
    out what biome that means. */
    int h = 0;
    int t = 0;
    while (t < (int)tempPercentiles.size() 
            && temperature > tempPercentiles[t]) {
        t++;
    }
    while (h < (int)humidityPercentiles.size()
            && humidity > humidityPercentiles[h]) {
        h++;
    }

    return (BiomeType)biomeData[t][h];
}
void Mapgen::generate(std::string filename, WorldType worldType) {
    /* Seed the random number generators. */
    map.seed = time(NULL);
    srand(map.seed);
    generator.seed(map.seed);

    /* Set the cylinder to get it's values from the scaled module. Of course,
    the scaled module will need to get its values from somewhere, too. */
    cylinder.SetModule(cylinderScale);

    /* Set the biome data vector. TODO: not hardcode filename? */
    std::ifstream infile("content/biomes.json");
    json j = json::parse(infile);
    biomeData = j["biomes"].get<std::vector<std::vector<int>>>();

    /* Run the appropriate function. */
    switch(worldType) {
        case WorldType::TEST : 
            break;
        case WorldType::SMOLTEST :
            break;
        case WorldType::EARTH :
            generateEarth();
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


