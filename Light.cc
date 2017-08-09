#include "Light.hh"

using json = nlohmann::json;

void from_json(const json &j, Light &light) {
    light.r = j["r"];
    light.g = j["g"];
    light.b = j["b"];
    light.skyIntensity = j["a"];
}
