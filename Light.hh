#ifndef LIGHT_HH
#define LIGHT_HH

#include <SDL2/SDL.h>
#include <algorithm>

using namespace std;

/* A struct to store the color (rgb) and intensity of light. */
struct Light {
    double intensity;
    Uint8 r;
    Uint8 g;
    Uint8 b;

    /* Sets this light equal to the sum of two other lights. */
    void sum(Light one, Light two) {
        // Take the max of each component.
        intensity = max(one.intensity, two.intensity);
        r = max(one.r, two.r);
        g = max(one.g, two.g);
        b = max(one.b, two.b);
    }

    /* Sets a new intensity and multiplies each value of rgb by it. */
    void setIntensity(double newIntensity) {
        intensity = newIntensity;
        r *= intensity;
        g *= intensity;
        b *= intensity;
    }
};

#endif