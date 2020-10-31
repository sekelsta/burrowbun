#include <cuda_runtime.h>
#include "noise_helpers.cuh"
#include "noisegen.cuh"
#include <math.h>


__device__
void adjustLocation(int xi, int yi, int width, int height, double scale, double *x_out, double *y_out, double *z_out) {
    // Wrap the map around a cylinder so the edges match
    double angle = xi * 2.0 * M_PI / width;

    // Transform to a cylinder of diameter width and height height
    // This is different from libnoise's cylinder, which is the unit, but I
    // think this is a better way of doing it (even though I have to 
    // readjust all the scales).
    double factor = width / (2.0 * M_PI);
    double x = cos(angle) * factor;
    double y = yi;
    double z = sin(angle) * factor;

    // Every single call includes turbulence and scale, so might as well do that here

    // Turbulence function modified from libnoise's turbulence module

    // Default value from libnoise
    int seed = 0;
    double power = 1.0;

    double x0, y0, z0;
    double x1, y1, z1;
    double x2, y2, z2;
    x0 = x + (12414.0 / 65536.0);
    y0 = y + (65124.0 / 65536.0);
    z0 = z + (31337.0 / 65536.0);
    x1 = x + (26519.0 / 65536.0);
    y1 = y + (18128.0 / 65536.0);
    z1 = z + (60493.0 / 65536.0);
    x2 = x + (53820.0 / 65536.0);
    y2 = y + (11213.0 / 65536.0);
    z2 = z + (44845.0 / 65536.0);
    double xDistort = x + (cuda_noise::GetPerlinValue(x0, y0, z0, seed)
    * power * factor);
    double yDistort = y + (cuda_noise::GetPerlinValue(x1, y1, z1, seed + 1)
    * power);
    double zDistort = z + (cuda_noise::GetPerlinValue(x2, y2, z2, seed + 2)
    * power * factor);
    
    // scale
    x *= scale;
    y *= scale;
    z *= scale;

    // And set outputs
    *x_out = x;
    *y_out = y;
    *z_out = z;
}
