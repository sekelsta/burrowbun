#include "Light.hh"
#include <SDL2/SDL.h>

using json = nlohmann::json;
using namespace std;

Light::operator SDL_Color() const {
    return {r, g, b, a};
}

void from_json(const json &j, Light &light) {
    light.r = j["r"];
    light.g = j["g"];
    light.b = j["b"];
    light.a = j["a"];
}
