#ifndef LIGHT_HH
#define LIGHT_HH

#include <nlohmann/json.hpp>
#include <algorithm>

struct SDL_Color;
struct DLight;

/* A struct to store the color (rgb) and intensity of light. */
struct Light {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    /* Constructors. */
    inline Light() {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }

    inline Light(const Light &l) {
        r = l.r;
        g = l.g;
        b = l.b;
        a = l.a;
    }

    inline Light(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        r(r), g(g), b(b), a(a) { }

    inline bool operator==(const Light &other) {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    inline bool operator!=(const Light &other) {
        return !(*this == other);
    }

    /* Sets this light equal to the sum of two other lights. */
    inline Light max(const Light &other) const {
        Light l;
        /* Take the max of each component. */
        l.r = std::max(r, other.r);
        l.g = std::max(g, other.g);
        l.b = std::max(b, other.b);
        l.a = std::max(a, other.a);
        return l;
    }

    /* Like sum but sets to the answer instead of converting. */
    void setmax(const Light &l) {
        *this = max(l);
    }

    /* Are any of the values smaller? */
    bool smaller(const Light &l) {
        return (r < l.r || g < l.g || b < l.b || a < l.a);
    }

    /* Return this light but with each value multiplied. */
    inline Light times(double coef) const {
        assert(r * coef <= 255);
        assert(g * coef <= 255);
        assert(b * coef <= 255);
        assert(a * coef <= 255);
        return Light(r * coef, g * coef, b * coef, a * coef);
    }

    /* Sum the light provided and the light from the sky (which has the 
    skyIntensity of the light provided and the color of the skyColor). */
    inline Light useSky(const Light &skyLight) const {
        return max(skyLight.setIntensity(a));
    }

    /* Sets a new intensity and multiplies each value of rgb by it. Should only
    be used just after r, g, and b have been set to their max for the given
    light color. Returns the result. */
    inline Light setIntensity(uint8_t intensity) const {
        Light l;
        l.a = intensity;
        l.r = r * intensity / 255;
        l.g = g * intensity / 255;
        l.b = b * intensity / 255;
        return l;
    }

    /* Converts to an SDL_Rect. */
    operator SDL_Color() const;
};

struct DLight {
    double r;
    double g;
    double b;
    double a;

    inline DLight() {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }

    inline DLight(const Light &light) {
        r = light.r;
        g = light.g;
        b = light.b;
        a = light.a;
    }

    inline DLight(double nr, double ng, double nb, double na) : 
        r(nr), g(ng), b(nb), a(na)
        { }

    inline bool operator==(const DLight &other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    inline bool operator!=(const DLight &other) const {
        return !(*this == other);
    }

    inline DLight &operator=(const DLight &other) {
        if (this == &other) {
            return *this;
        }
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }

    inline DLight divide(const DLight &other) const {
        assert(other.r != 0);
        assert(other.g != 0);
        assert(other.b != 0);
        assert(other.a != 0);
        DLight answer = {r / other.r, g / other.g, b / other.b, a / other.a};
        return answer;
    }

    inline DLight timesa() const {
        return DLight(r * a, g * a, b * a, a);
    }

    inline DLight max(const DLight &other) const {
        return DLight(std::max(r, other.r), std::max(g, other.g), 
            std::max(b, other.b), std::max(a, other.a));
    }

    /* Return true if this increases any of the values. */
    inline bool setmax(const DLight &other) {
        DLight m = max(other);
        if (*this == m) {
            return true;
        }
        *this = m;
        return false;
    }
};

void from_json(const nlohmann::json &j, Light &light);

#endif
