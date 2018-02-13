#include "Stat.hh"

using json = nlohmann::json;

void Stat::regenerate() {
    double total = 0;
    if (ticksUntilRegen < 0) {
        total = percentRegen * (maxStat + part);
        total += quadraticRegen * full;
        total *= -1 * timeRegen * ticksUntilRegen;
    }
    total += linearRegen;
    addFull(total);
}

// Constructor
Stat::Stat() {
    full = 0;
    part = 0;
    maxStat = 0;
    linearRegen = 0;
    percentRegen = 0;
    quadraticRegen = 0;
    timeRegen = 0;
    ticksUntilRegen = 0;
    baseRegenTicks = 1;
}

void from_json(const json &j, Stat &stat) {
    stat.maxStat = j["maxStat"];
    stat.full = j["full"];
    stat.part = j["part"];
    stat.linearRegen = j["linearRegen"];
    stat.percentRegen = j["percentRegen"];
    stat.quadraticRegen = j["quadraticRegen"];
    stat.timeRegen = j["timeRegen"];
    stat.ticksUntilRegen = 0;
    stat.baseRegenTicks = j["baseRegenTicks"];
}
