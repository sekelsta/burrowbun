#include "Entity.hh"

using json = nlohmann::json;

// Constructor
Entity::Entity(std::string filename) : movable::Movable(filename) {
    /* Open the json file. */
    std::ifstream infile(filename);
    /* Put data in json. */
    json j = json::parse(infile);
    maxFallDistance = j["maxFallDistance"];
    health = j["health"].get<Stat>();
    fullness = j["fullness"].get<Stat>();
    mana = j["mana"].get<Stat>();
    assert(sprite.name != "");
}

Entity::Entity() {};

// Virtual destructor
Entity::~Entity() {}

// Recieve a certain amount of raw damage and wounds, before taking defense
// into account
void Entity::takeDamage(int normal, int wounds) {
    // TODO: use defense
    health.addFull(-1 * normal);
    health.addPart(-1 * wounds);
    // TODO: maybe not here, but die if no health
}

// Calculate how much fall damage to take
void Entity::takeFallDamage() {
    // pixelsFallen will only be non-zero when we need to calculate fall damage
    if (pixelsFallen > maxFallDistance && maxFallDistance != -1) {
        int effectiveDistance = (pixelsFallen - maxFallDistance);
        int damage = effectiveDistance * effectiveDistance / 256 / 8;
        damage += effectiveDistance / 8;
        takeDamage(damage, 0);
    }
}

/* Do the things! */
void Entity::update() {
    health.update();
    fullness.update();
    mana.update();
}

/* Make an entity from a json. */
void from_json(const json &j, Entity &entity) {
    entity.maxFallDistance = j["maxFallDistance"];
    entity.health = j["health"].get<Stat>();
    entity.fullness = j["fullness"].get<Stat>();
    entity.mana = j["mana"].get<Stat>();
}
