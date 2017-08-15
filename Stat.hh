#ifndef STAT_HH
#define STAT_HH

#include <iostream>
#include "json.hpp"

/* A struct for having a stat, such as health or mana. */
struct Stat {
    // The actual stats
    double maxStat;
    double full;
    double part;

    /* Affects how fast it regenerates. */
    double linearRegen;
    double percentRegen;
    double quadraticRegen;
    /* How much faster more time passing since being damaged makes it 
    regenerate. */
    double timeRegen;
    int ticksUntilRegen;
    int baseRegenTicks;

private:
    /* Slightly increase the amount of fullStat. */
    void regenerate();

public:
    // Constructor
    Stat();

    // Set the amount of the stat
    void setFull(double newValue);

    // Set the temporary cap (which prevents the stat from regenerating 
    // completely)
    void setPart(double newValue);

    // Add amount to the full part
    void addFull(double amount);

    // Add amount to the part part
    void addPart(double amount);

    // Set the stat to as high as it can go
    void fill();

    /* Reset the time until it can regenerate again. */
    void resetRegen();

    /* Do the things! */
    void update();
};

/* Make a statbar from a json. */
void from_json(const nlohmann::json &j, Stat &stat);

#endif
