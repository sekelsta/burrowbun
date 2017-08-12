#include "Stat.hh"

using json = nlohmann::json;

/* Slightly increase the amount of fullStat. */
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

// Set the amount of the stat
void Stat::setFull(double newValue) {
    // Can't set it below 0 or above the max
    newValue = std::max(0.0, newValue);
    newValue = std::min(maxStat, newValue);
    full = newValue;
    // And you can't recover a stat past the temporary cap
    if (full > part) {
        full = part;
    }
}

// Set the temporary cap (which prevents the stat from regenerating 
// completely)
void Stat::setPart(double newValue) {
    newValue  = std::max(0.0, newValue);
    newValue = std::min(maxStat, newValue);
    part = newValue;
    if (full > part) {
        setFull(part);
    }
}

// Add amount to the full part
void Stat::addFull(double amount) {
    /* If we lost the stat, we shouldn't start regenerating right away. */
    if (amount < 0) {
        resetRegen();
    }
    setFull(full + amount);
}

// Add amount to the part part
void Stat::addPart(double amount) {
    /* Again, reset regen. */
    if (amount < 0) {
        resetRegen();
    }
    setPart(part + amount);
}

// Set the stat to as high as it can go
void Stat::fill() {
    setPart(maxStat);
    setFull(maxStat);
}

/* Reset the time until it can regenerate again. */
void Stat::resetRegen() {
    ticksUntilRegen = baseRegenTicks;
}

/* Do the things! */
void Stat::update() {
    regenerate();
    ticksUntilRegen -= 1;
}

/* Make a statbar from a json file. */
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
