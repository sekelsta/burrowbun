#ifndef DAMAGE_HH
#define DAMAGE_HH

#include <random>
#include "json.hpp"

/* What type of damage is being dealt. */
enum class DamageType {
    SLASHING,
    PIERCING,
    BLUDGEONING,
    FIRE,
    COLD,
    LIGHTNING,
    NECROTIC
};

struct Damage {
    int minDamage;
    int maxDamage;
    /* Mean damage = max * balance + min * (1 - balance). */
    double balance;
    /* Probability of a critical hit from this damage source. */
    double criticalChance;
    /* On a critical hit, damage is multipled by this. */
    double criticalAmount;
    /* The injury applied will be end damage * woundRate. */
    double minWounds;
    double maxWounds;
    DamageType type;

    /* Use min, max, and balance to pick a random amount of damage. This will
    not be consistent across calls (i. e. calling it multiple times, even on
    the same object, will return different values). */
    inline double getBaseDamage() const {
        const float minRange = -2;
        const float maxRange = 2;
        /* Our distribution will have a mean of 0 and a standard deviation of
        1, since it's private and none of our methods change it. We'll 
        bound it between -2 and 2, since 95% of the values will be 
        between there already. */
        double num = distribution(generator);
        /* We want our mean damage to be balance * max + (1 - balance) * min.
        To get that, we'll linearly map values from [-2, 0] to 
        [min, mean] and from [0, 2] to [mean, max]. */
        double mean = balance * maxDamage;
        mean += (1 - balance) * minDamage;
        if (num < 0) {
            double scaled = mean - num / minRange * (minDamage - mean); 
            return std::max(scaled, (double)minDamage);
        }
        else {
            double scaled = mean + num / maxRange * (maxDamage - mean);
            return std::min(scaled, (double)maxDamage);
        }
    }

private:
    /* To generate random numbers in a bell curve (mean 0, stdev 1). */
    static std::default_random_engine generator;
    static std::normal_distribution<double> distribution;
};


void from_json(const nlohmann::json &j, Damage &damage);
#endif
