// Modified from libnoise

// noisegen.cpp
//
// Copyright (C) 2003, 2004 Jason Bevins
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//
#include <cuda_runtime.h>
#include "noisegen.cuh"
#include "interp.cuh"
#include "vectortable.cuh"

// Constants used by the current version of libnoise.
__constant__ int X_NOISE_GEN = 1619;
__constant__ int Y_NOISE_GEN = 31337;
__constant__ const int Z_NOISE_GEN = 6971;
__constant__ const int SEED_NOISE_GEN = 1013;
__constant__ const int SHIFT_NOISE_GEN = 8;

// The default values from libnoise's perlin module
__constant__ int octaveCount = 6;
__constant__ double frequency = 1.0;
__constant__ double lacunarity = 2.0;
__constant__ double persistence = 0.5;

// For ridged multi
__constant__ double spectralWeights[6] = {1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125};

// Slightly modified from libnoise's perlin.cpp
__device__
double cuda_noise::GetPerlinValue (double x, double y, double z, int seed)
{
  double value = 0.0;
  double signal = 0.0;
  double curPersistence = 1.0;
  double nx, ny, nz;
  int curSeed;

  x *= frequency;
  y *= frequency;
  z *= frequency;

  for (int curOctave = 0; curOctave < octaveCount; curOctave++) {

    // Make sure that these floating-point values have the same range as a 32-
    // bit integer so that we can pass them to the coherent-noise functions.
    nx = MakeInt32Range (x);
    ny = MakeInt32Range (y);
    nz = MakeInt32Range (z);

    // Get the coherent-noise value from the input value and add it to the
    // final result.
    curSeed = (seed + curOctave) & 0xffffffff;
    signal = GradientCoherentNoise3D (nx, ny, nz, curSeed);
    value += signal * curPersistence;

    // Prepare the next octave.
    x *= lacunarity;
    y *= lacunarity;
    z *= lacunarity;
    curPersistence *= persistence;
  }

  return value;
}

//Slightly modified from libnoise's RidgeMulti module
__device__
double cuda_noise::GetRidgedMultiValue(double x, double y, double z, int seed)
{
  x *= frequency;
  y *= frequency;
  z *= frequency;

  double signal = 0.0;
  double value  = 0.0;
  double weight = 1.0;

  // These parameters should be user-defined; they may be exposed in a
  // future version of libnoise.
  double offset = 1.0;
  double gain = 2.0;

  for (int curOctave = 0; curOctave < octaveCount; curOctave++) {

    // Make sure that these floating-point values have the same range as a 32-
    // bit integer so that we can pass them to the coherent-noise functions.
    double nx, ny, nz;
    nx = MakeInt32Range (x);
    ny = MakeInt32Range (y);
    nz = MakeInt32Range (z);

    // Get the coherent-noise value.
    int curSeed = (seed + curOctave) & 0x7fffffff;
    signal = GradientCoherentNoise3D (nx, ny, nz, curSeed);

    // Make the ridges.
    signal = fabs (signal);
    signal = offset - signal;

    // Square the signal to increase the sharpness of the ridges.
    signal *= signal;

    // The weighting from the previous octave is applied to the signal.
    // Larger values have higher weights, producing sharp points along the
    // ridges.
    signal *= weight;

    // Weight successive contributions by the previous signal.
    weight = signal * gain;
    if (weight > 1.0) {
      weight = 1.0;
    }
    if (weight < 0.0) {
      weight = 0.0;
    }

    // Add the signal to the output value.
    value += (signal * spectralWeights[curOctave]);

    // Go to the next octave.
    x *= lacunarity;
    y *= lacunarity;
    z *= lacunarity;
  }

  return (value * 1.25) - 1.0;
}

__device__
double cuda_noise::GradientCoherentNoise3D (double x, double y, double z, int seed)
{
  // Create a unit-length cube aligned along an integer boundary.  This cube
  // surrounds the input point.
  int x0 = (x > 0.0? (int)x: (int)x - 1);
  int x1 = x0 + 1;
  int y0 = (y > 0.0? (int)y: (int)y - 1);
  int y1 = y0 + 1;
  int z0 = (z > 0.0? (int)z: (int)z - 1);
  int z1 = z0 + 1;

  // Map the difference between the coordinates of the input value and the
  // coordinates of the cube's outer-lower-left vertex onto an S-curve.
  double xs = 0, ys = 0, zs = 0;

  xs = SCurve3 (x - (double)x0);
  ys = SCurve3 (y - (double)y0);
  zs = SCurve3 (z - (double)z0);

  // Now calculate the noise values at each vertex of the cube.  To generate
  // the coherent-noise value at the input point, interpolate these eight
  // noise values using the S-curve value as the interpolant (trilinear
  // interpolation.)
  double n0, n1, ix0, ix1, iy0, iy1;
  n0   = GradientNoise3D (x, y, z, x0, y0, z0, seed);
  n1   = GradientNoise3D (x, y, z, x1, y0, z0, seed);
  ix0  = LinearInterp (n0, n1, xs);
  n0   = GradientNoise3D (x, y, z, x0, y1, z0, seed);
  n1   = GradientNoise3D (x, y, z, x1, y1, z0, seed);
  ix1  = LinearInterp (n0, n1, xs);
  iy0  = LinearInterp (ix0, ix1, ys);
  n0   = GradientNoise3D (x, y, z, x0, y0, z1, seed);
  n1   = GradientNoise3D (x, y, z, x1, y0, z1, seed);
  ix0  = LinearInterp (n0, n1, xs);
  n0   = GradientNoise3D (x, y, z, x0, y1, z1, seed);
  n1   = GradientNoise3D (x, y, z, x1, y1, z1, seed);
  ix1  = LinearInterp (n0, n1, xs);
  iy1  = LinearInterp (ix0, ix1, ys);

  return LinearInterp (iy0, iy1, zs);
}

__device__
double cuda_noise::GradientNoise3D (double fx, double fy, double fz, int ix,
  int iy, int iz, int seed)
{
  // Randomly generate a gradient vector given the integer coordinates of the
  // input value.  This implementation generates a random number and uses it
  // as an index into a normalized-vector lookup table.
  int vectorIndex = (
      X_NOISE_GEN    * ix
    + Y_NOISE_GEN    * iy
    + Z_NOISE_GEN    * iz
    + SEED_NOISE_GEN * seed)
    & 0xffffffff;
  vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
  vectorIndex &= 0xff;

  double xvGradient = g_randomVectors[(vectorIndex << 2)    ];
  double yvGradient = g_randomVectors[(vectorIndex << 2) + 1];
  double zvGradient = g_randomVectors[(vectorIndex << 2) + 2];

  // Set up us another vector equal to the distance between the two vectors
  // passed to this function.
  double xvPoint = (fx - (double)ix);
  double yvPoint = (fy - (double)iy);
  double zvPoint = (fz - (double)iz);

  // Now compute the dot product of the gradient vector with the distance
  // vector.  The resulting value is gradient noise.  Apply a scaling value
  // so that this noise value ranges from -1.0 to 1.0.
  return ((xvGradient * xvPoint)
    + (yvGradient * yvPoint)
    + (zvGradient * zvPoint)) * 2.12;
}

__device__
int cuda_noise::IntValueNoise3D (int x, int y, int z, int seed)
{
  // All constants are primes and must remain prime in order for this noise
  // function to work correctly.
  int n = (
      X_NOISE_GEN    * x
    + Y_NOISE_GEN    * y
    + Z_NOISE_GEN    * z
    + SEED_NOISE_GEN * seed)
    & 0x7fffffff;
  n = (n >> 13) ^ n;
  return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
}

__device__
double cuda_noise::ValueCoherentNoise3D (double x, double y, double z, int seed)
{
  // Create a unit-length cube aligned along an integer boundary.  This cube
  // surrounds the input point.
  int x0 = (x > 0.0? (int)x: (int)x - 1);
  int x1 = x0 + 1;
  int y0 = (y > 0.0? (int)y: (int)y - 1);
  int y1 = y0 + 1;
  int z0 = (z > 0.0? (int)z: (int)z - 1);
  int z1 = z0 + 1;

  // Map the difference between the coordinates of the input value and the
  // coordinates of the cube's outer-lower-left vertex onto an S-curve.
  double xs = 0, ys = 0, zs = 0;

  xs = SCurve3 (x - (double)x0);
  ys = SCurve3 (y - (double)y0);
  zs = SCurve3 (z - (double)z0);

  // Now calculate the noise values at each vertex of the cube.  To generate
  // the coherent-noise value at the input point, interpolate these eight
  // noise values using the S-curve value as the interpolant (trilinear
  // interpolation.)
  double n0, n1, ix0, ix1, iy0, iy1;
  n0   = ValueNoise3D (x0, y0, z0, seed);
  n1   = ValueNoise3D (x1, y0, z0, seed);
  ix0  = LinearInterp (n0, n1, xs);
  n0   = ValueNoise3D (x0, y1, z0, seed);
  n1   = ValueNoise3D (x1, y1, z0, seed);
  ix1  = LinearInterp (n0, n1, xs);
  iy0  = LinearInterp (ix0, ix1, ys);
  n0   = ValueNoise3D (x0, y0, z1, seed);
  n1   = ValueNoise3D (x1, y0, z1, seed);
  ix0  = LinearInterp (n0, n1, xs);
  n0   = ValueNoise3D (x0, y1, z1, seed);
  n1   = ValueNoise3D (x1, y1, z1, seed);
  ix1  = LinearInterp (n0, n1, xs);
  iy1  = LinearInterp (ix0, ix1, ys);
  return LinearInterp (iy0, iy1, zs);
}

__device__
double cuda_noise::ValueNoise3D (int x, int y, int z, int seed)
{
  return 1.0 - ((double)IntValueNoise3D (x, y, z, seed) / 1073741824.0);
}

