#ifndef LIGHT_HH
#define LIGHT_HH

#include <SDL2/SDL.h>
#include <algorithm>

using namespace std;

/* A struct to store the color (rgb) and intensity of light. */
struct Light {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 skyIntensity;

    /* Sets this light equal to the sum of two other lights. */
    void sum(const Light &one, const Light &two) {
        // Take the max of each component.
        r = max(one.r, two.r);
        g = max(one.g, two.g);
        b = max(one.b, two.b);
    }

    /* Sum the light provided and the light from the sky (which has the 
    skyIntensity of the light provided and the color of the skyColor). */
    void useSky(const Light &light, const Light &skyColor) {
        Light skyLight = skyColor;
        skyLight.setIntensity(light.skyIntensity);
        sum(light, skyLight);
    }

    /* Sets a new intensity and multiplies each value of rgb by it. Should only
    be used just after r, g, and b have been set to their max for the given
    light color. */
    void setIntensity(Uint8 intensity) {
        skyIntensity = intensity;
        r = r * intensity / 255;
        g = g * intensity / 255;
        b = b * intensity / 255;
    }
};

#endif
