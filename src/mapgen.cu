
#include <cuda_runtime.h>
#include <cuda.h>
#include "mapgen.cuh"
#include "noisegen.cuh"
#include "noise_helpers.cuh"
#include "helper_cuda.h"

#define THREADS_PER_BLOCK 128;

#define LAND_SLOPE 20
#define SHORE_SIZE 40

#define AIR 0
#define WATER 1
#define DIRT 2
#define STONE 8
#define GRANITE 9
#define BASALT 10
#define PERIDOTITE 13
#define GLOWSTONE 22

/*Source: 
http://stackoverflow.com/questions/17399119/
cant-we-use-atomic-operations-for-floating-point-variables-in-cuda
*/
/*__device__ static float atomicMax(float* address, float val)
{
    int* address_as_i = (int*) address;
    int old = *address_as_i, assumed;
    do {
        assumed = old;
        old = ::atomicCAS(address_as_i, assumed,
            __float_as_int(::fmaxf(val, __int_as_float(assumed))));
    } while (assumed != old);
    return __int_as_float(old);
}*/

__device__
int getX(int width) {
    return (blockIdx.x * blockDim.x + threadIdx.x) % width;
}

__device__
int getY(int width) {
    return (blockIdx.x * blockDim.x + threadIdx.x) / width;
}

__device__
double surface_modifier(int x, int y, int width, int height) {
    int baseHeight = height * 0.8;
    int seafloorLevel = height * 0.5;
    int shoreline = width * 0.25;
    int abyss = width * 0.35;
    double steepness = 50;
    // Baseline is at baseHeight
    double surface = (y - baseHeight) / steepness;
    // Continental modifier
    double quadratic = LAND_SLOPE * ((x - width / 2.0) 
            * (x - width / 2.0)) / (width * width);
    // Oceanic modifier
    double linear = 5 * abs(width / 2.0 - x) / width;
    // Ocean floor's baseline is at seafloorLevel and it gets a little deeper linearly
    int depth = (baseHeight - seafloorLevel) / steepness;
    if (abs(width / 2.0 - x) > abyss) {
        surface += depth + linear;
    }
    // Interpolate between land height and seafloor depth
    else if (abs(width / 2.0 - x) > shoreline) {
        double dist = (abs(width / 2.0 - x) - shoreline) 
                        / (abyss - shoreline);
        double interp = dist < 0.5? pow(0.5 - dist, 1 / 3.0) 
            : -1 * pow(dist - 0.5, 1 / 3.0);
        interp /= 2 * 0.7937; //cube root of 0.5
        interp += 0.5;
        surface += (1 - interp) * (depth + linear);
        surface += interp * quadratic;
    }
    // This gives a slight curve to the land
    else {
        surface += quadratic;
    }
    return surface;
}

// Ideally would increase the turbulence on here a bit
__device__
double cave_noise(int seed, int xi, int yi, int width, int height) {
    seed += 20;
    double scale = 20.0 / 2048.0;
    double y_scale = 2.0;
    double x, y, z;
    adjustLocation(xi, yi, width, height, scale, &x, &y, &z);
    y *= y_scale;

    return cuda_noise::GetRidgedMultiValue(x, y, z, seed);
}

// This is mapgen.cc's tunnel
__device__
double cavern_noise(int seed, int xi, int yi, int width, int height) {
    seed += 30;
    double scale = 2.7 / 2048.0;
    double y_scale = 3.0;
    double x, y, z;
    adjustLocation(xi, yi, width, height, scale, &x, &y, &z);
    y *= y_scale;

    return cuda_noise::GetRidgedMultiValue(x, y, z, seed);
}

__device__
double surface_noise(int seed, int xi, int yi, int width, int height) {
    seed += 40;
    double scale = 3.0 / 2048.0;
    double x, y, z;
    adjustLocation(xi, yi, width, height, scale, &x, &y, &z);

    return cuda_noise::GetPerlinValue(x, y, z, seed);
}

// This is mapgen.cc's wetness + humidity
__device__
double wetness_noise(int seed, int xi, int yi, int width, int height) {
    int wet_seed = seed + 50; // scaled by 0.015
    int humid_seed = seed - 10; // scaled by 0.0014
    double wet_scale = 15 * 3.0 / 2048;
    double humid_scale = 1.4 * 3.0 / 2048;

    double x, y, z;
    adjustLocation(xi, yi, width, height, humid_scale, &x, &y, &z);
    double value = cuda_noise::GetPerlinValue(x, y, z, humid_seed);
    adjustLocation(xi, yi, width, height, wet_scale, &x, &y, &z);
    return value + cuda_noise::GetPerlinValue(x, y, z, wet_seed);
}

__device__
double felsic_noise(int seed, int xi, int yi, int width, int height) {
    seed += 60;
    double scale = 3.0 / 2048.0;
    
    double x, y, z;
    adjustLocation(xi, yi, width, height, scale, &x, &y, &z);

    return cuda_noise::GetPerlinValue(x, y, z, seed);
}

__device__
bool is_cavern(double surface, double cavern, int y, int height) {
    const double cavernBoundary = 0.66;
    const double cavernLimit = -1.63;
    int cavernHeight = height * 0.5;
    double s = max(surface, (y - cavernHeight) / 50.0 + surface);

    return cavern > cavernBoundary && s / 2.0 - cavern < cavernLimit;
}

__device__
char choose_rock(int seed, int xi, int yi, int width, int height) {
    const double basaltLimit = -0.31;
    const double graniteLimit = 0.31;
    const double peridotLimit = -0.73;
    const int seafloorLevel = height * 0.5;
    const int baseHeight = height * 0.8;
    char rock = STONE;

    double interp = 0;
    double felsic = felsic_noise(seed, xi, yi, width, height);
    // The CPU code relies on the surfaces vector, which could still be 
    // being written, so I'm using a quick approximation of surface_modifier instead
    int shoreline = width * 0.25;
    int abyss = width * 0.35;
    int surface;
    if (abs(xi - (width / 2.0)) < shoreline) {
        surface = baseHeight;
    }
    else if (abs(xi - (width / 2.0)) > abyss) {
        surface = seafloorLevel;
    }
    else {
        interp = (abs(xi - (width / 2.0)) - shoreline) / (abyss - shoreline);
        surface = interp * seafloorLevel + (1 - interp) * baseHeight;
    }

    // Now mirror it
    /* Adjust so that continental plates tend to be made of 
    granite, while oceanic plates tend to be made of basalt, 
    and the upper mantle is peridotite. */
    if (seafloorLevel - yi > surface - seafloorLevel) {
        double dist = surface > seafloorLevel? 
            2 * seafloorLevel - surface : surface;
        interp = abs((dist - yi) / dist);
        felsic -= abs(peridotLimit + basaltLimit) / 2.0 + interp;
    }
    else if (seafloorLevel - yi == surface - seafloorLevel) {
        // pass
    }
    else {
        double dist = 2 * (surface - seafloorLevel);
        interp = abs((dist - (surface - yi)) / dist);
        felsic += abs(graniteLimit) / 2.0 + 0.2 * interp;
    }

    if (felsic < peridotLimit && interp - 0.7 > 0.25 * felsic) {
        rock = PERIDOTITE;
    }
    else if (felsic < basaltLimit) {
        rock = BASALT;
    }
    else if (felsic > graniteLimit) {
        rock = GRANITE;
    }
    return rock;
}

__global__
void dev_generate_terrain(int seed, int width, int height, char *map, int *surfaces) {
    const double caveBoundary = 0.55;
    const double caveLimit = -1.24;
    const double waterLimit = 0.83;

    const int i = getX(width);
    const int j = getY(width);
    
    char tile = STONE;
    double surface = surface_modifier(i, j, width, height) 
                    + surface_noise(seed, i, j, width, height);
    double cave = cave_noise(seed, i, j, width, height);
    double cavern = cavern_noise(seed, i, j, width, height);
    if (surface > 0
            || cave > caveBoundary && surface - cave < caveLimit) {
        tile = AIR;
    }
    else if (is_cavern(surface, cavern, j, height)) {
        if (j + 1 < height) {
            double up_surface = surface_modifier(i, j + 1, width, height) 
                        + surface_noise(seed, i, j + 1, width, height);
            double up_cavern = cavern_noise(seed, i, j + 1, width, height);
            if (!is_cavern(up_surface, up_cavern, j + 1, height)) {
                tile = GLOWSTONE;
            }
            else {
                tile = AIR;
            }
        }
    }
    else {
        atomicMax(&surfaces[i], j);
        tile = choose_rock(seed, i, j, width, height);
    }

    if (tile == AIR && wetness_noise(seed, i, j, width, height) > waterLimit) {
        tile = WATER;
    }

    map[blockIdx.x * blockDim.x + threadIdx.x] = tile;
}

char *generate_terrain(int seed, int width, int height, int *surfaces) {
    char *dev_map;
    int *dev_surfaces;
    CUDA_CALL(cudaMalloc(&dev_map, width * height * sizeof(char)));
    CUDA_CALL(cudaMalloc(&dev_surfaces, width * sizeof(int)));

    int n_blocks = width * height / THREADS_PER_BLOCK;
    dim3 num_blocks(n_blocks);
    dim3 num_threads(128);

    //int seaLevel = height * 0.72;
 
    dev_generate_terrain<<<num_blocks, num_threads>>>(seed, width, height, dev_map, dev_surfaces);

    char *map = (char *)malloc(width * height * sizeof(char));
    CUDA_CALL(cudaMemcpy(map, dev_map, width * height * sizeof(char), cudaMemcpyDeviceToHost));
    CUDA_CALL(cudaMemcpy(surfaces, dev_surfaces, width * sizeof(int), cudaMemcpyDeviceToHost));
    return map;
}

