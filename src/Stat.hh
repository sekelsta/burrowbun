#ifndef STAT_HH
#define STAT_HH

#include <iostream>
#include "json.hh"

/* A struct for having a stat, such as health or mana. */
struct Stat {
    /* The actual stats. */
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

    /* Set the amount of the stat. */
    inline void setFull(double newValue) {
        /* Can't set it below 0 or above the max. */
        newValue = std::max(0.0, newValue);
        newValue = std::min(maxStat, newValue);
        full = newValue;
        /* And you can't recover a stat past the temporary cap. */
        if (full > part) {
            full = part;
        }
    }

    /* Set the temporary cap (which prevents the stat from regenerating 
    completely). */
    inline void setPart(double newValue) {
        newValue  = std::max(0.0, newValue);
        newValue = std::min(maxStat, newValue);
        part = newValue;
        if (full > part) {
            setFull(part);
        }
    }

    /* Add amount to the full part. */
    inline void addFull(double amount) {
        /* If we lost the stat, we shouldn't start regenerating right away. */
        if (amount < 0) {
            resetRegen();
        }
        setFull(full + amount);
    }

    /* Add amount to the part part. */
    inline void addPart(double amount) {
        /* Again, reset regen. */
        if (amount < 0) {
            resetRegen();
        }
        setPart(part + amount);
    }

    /* Set the stat to as high as it can go. */
    inline void fill() {
        setPart(maxStat);
        setFull(maxStat);
    }

    /* Reset the time until it can regenerate again. */
    inline void resetRegen() {
        ticksUntilRegen = baseRegenTicks;
    }

    /* Do the things! */
    inline void update() {
        regenerate();
        ticksUntilRegen -= 1;
    }
};

/* Make a statbar from a json. */
void from_json(const nlohmann::json &j, Stat &stat);

#endif
