#include "Entity.hh"

// Constructor
Entity::Entity(std::string filename) : Movable(filename) {
    // Initialize the maxFall damage, but whatever class inherits probably 
    // wats to put it's own value
    maxFallDistance = 0;

    // Set the colors of the statbars
    health.fullColor.r = 0xFF;
    health.fullColor.g = 0x10;
    health.fullColor.b = 0x20;
    health.partColor.r = 0x40;
    health.partColor.g = 0x00;
    health.partColor.b = 0x10;
    health.emptyColor.r = 0x00;
    health.emptyColor.g = 0x00;
    health.emptyColor.b = 0x00;
    fullness.fullColor.r = 0xF0;
    fullness.fullColor.g = 0xF0;
    fullness.fullColor.b = 0x00;
    fullness.partColor.r = 0x40;
    fullness.partColor.g = 0x40;
    fullness.partColor.b = 0x00;
    fullness.emptyColor = health.emptyColor; // black
    mana.fullColor.r = 0x18;
    mana.fullColor.g = 0x30;
    mana.fullColor.b = 0xFF;
    mana.partColor.r = 0x04;
    mana.partColor.g = 0x14;
    mana.partColor.b = 0x66;
    mana.emptyColor = health.emptyColor; // still black
    /* Set regeneration rates. */
    health.percentRegen = 0.00001;
    health.baseRegenTicks = 80;
    health.timeRegen = 0.005;
    fullness.linearRegen = -0.0007;
    mana.linearRegen = 0.001;
    mana.quadraticRegen = 0.00001;
    mana.timeRegen = 0.0001;
    mana.baseRegenTicks = 60;
}

// Virtual destructor
Entity::~Entity() {}

// Recieve a certain amount of raw damage and wounds, before taking defense
// into account
void Entity::takeDamage(int normal, int wounds) {
    // TODO: use defense
    health.setFull(health.fullStat - normal);
    health.setPart(health.partStat - wounds);
    /* Reset regeneration if it was a non-zero amount of damage. */
    if (normal || wounds) {
        health.resetRegen();
    }
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
