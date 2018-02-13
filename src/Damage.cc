#include "Damage.hh"

std::default_random_engine Damage::generator;
std::normal_distribution<double> Damage::distribution;

void from_json(const nlohmann::json &j, Damage &damage) {
    damage.minDamage = j["minDamage"];
    damage.maxDamage = j["maxDamage"];
    damage.balance = j["balance"];
    damage.criticalChance = j["criticalChance"];
    damage.criticalAmount = j["criticalAmount"];
    damage.minWounds = j["minWounds"];
    damage.maxWounds = j["maxWounds"];
    damage.type = (DamageType)((int)(j["type"]));
}

