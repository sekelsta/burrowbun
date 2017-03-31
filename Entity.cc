#include "Entity.hh"

// Constructor
Entity::Entity() {
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
    hunger.fullColor.r = 0xF0;
    hunger.fullColor.g = 0xF0;
    hunger.fullColor.b = 0x00;
    hunger.partColor.r = 0x88;
    hunger.partColor.g = 0x44;
    hunger.partColor.b = 0x00;
    hunger.emptyColor = health.emptyColor; // black
    mana.fullColor.r = 0x10;
    mana.fullColor.g = 0x28;
    mana.fullColor.b = 0xFF;
    mana.partColor.r = 0x08;
    mana.partColor.g = 0x18;
    mana.partColor.b = 0x60;
    mana.emptyColor = health.emptyColor; // still black
}

// Recieve a certain amount of raw damage and wounds, before taking defense
// into account
void Entity::takeDamage(int normal, int wounds) {
    // TODO: use defense
    health.setFull(health.fullStat - normal);
    health.setPart(health.partStat - wounds);
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
