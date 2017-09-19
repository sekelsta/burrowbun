#include "Entity.hh"
#include "filepaths.hh"

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
    invincibilityTime = j["invincibilityTime"];
    invincibilityLeft = 0;
    assert(sprite.name != "");
    isFacingRight = true;
    sprites = j["sprites"].get<std::vector<Sprite>>();
    for (unsigned int i = 0; i < sprites.size(); i++) {
        sprites[i].loadTexture(MOVABLE_SPRITE_PATH);
    }
    run.emplace_back(j["run_left"], MOVABLE_SPRITE_PATH);
    run.emplace_back(j["run_right"], MOVABLE_SPRITE_PATH);
}

Entity::Entity() {};

// Virtual destructor
Entity::~Entity() {}

// Recieve a certain amount of raw damage and wounds, before taking defense
// into account
void Entity::takeDamage(const Damage &damage) {
    /* Do nothing when applicable. */
    if (invincibilityLeft >= 0) {
        return;
    }
    if (damage.maxDamage == 0) {
        return;
    }

    /* Not allowed to take 0 damage. */
    assert(damage.minDamage >= 1);

    double baseDamage = damage.getBaseDamage();
    assert(baseDamage >= 1);
    if ((double)(rand() % 100) < 100.0 * damage.criticalChance) {
        baseDamage *= damage.criticalAmount;
    }

    // TODO: use defense
    health.addFull(-1 * (int)baseDamage);
    if (damage.maxWounds > 0) {
        double woundRate = (double)(rand() % (int)(damage.maxWounds * 100));
        woundRate /= 100.0;
        woundRate += damage.minWounds;
        health.addPart(-1 * (int)(baseDamage * woundRate));
    }
    invincibilityLeft = invincibilityTime;
    // TODO: maybe not here, but die if no health
}

// Calculate how much fall damage to take
void Entity::takeFallDamage() {
    // pixelsFallen will only be non-zero when we need to calculate fall damage
    if (pixelsFallen > maxFallDistance && maxFallDistance != -1) {
        int effectiveDistance = (pixelsFallen - maxFallDistance);
        Damage damage;
        damage.minDamage = effectiveDistance * effectiveDistance / 256 / 8;
        damage.minDamage = std::max(damage.minDamage, 1);
        damage.maxDamage = damage.minDamage;
        damage.maxDamage += effectiveDistance / 8;
        damage.balance = 0.5;
        damage.minWounds = 0;
        damage.maxWounds = 0;
        damage.criticalChance = 0;
        damage.criticalAmount = 1;
        damage.type = DamageType::BLUDGEONING;
        takeDamage(damage);
    }
}

/* Do the things! */
void Entity::update() {
    health.update();
    fullness.update();
    mana.update();
    invincibilityLeft--;
    /* Check if we need to be facing a certain way. */
    if (getVelocity().x > 0.0001) {
        isFacingRight = true;
    }
    else if (getVelocity().x < -0.0001) {
        isFacingRight = false;
    }
}

void Entity::render(const Rect &camera) {
    /* TODO: this is copypasta code */
    // Make sure the renderer draw color is set to white
    Renderer::setColorWhite();

    SDL_Rect rectTo;


    /* So as not to use == with a double. */
    if (abs(getVelocity().x) < 0.0001) {
        rectTo.x = x;
        rectTo.y = y;
        rectTo.w = sprites[isFacingRight].rect.w;
        rectTo.h = sprites[isFacingRight].rect.h;
        // Convert the rectangle to screen coordinates
        convertRect(rectTo, camera);

        // Draw!
        // TODO: check whether it's actually anywhere near the screen
        sprites[isFacingRight].render(&rectTo);
        return;
    }
    /* Otherwise we use the run sprite. */
    rectTo = run[isFacingRight].getRect();
    rectTo.x = x;
    rectTo.y = y;
    convertRect(rectTo, camera);
    run[isFacingRight].render(rectTo);
}

/* Make an entity from a json. */
void from_json(const json &j, Entity &entity) {
    entity.maxFallDistance = j["maxFallDistance"];
    entity.health = j["health"].get<Stat>();
    entity.fullness = j["fullness"].get<Stat>();
    entity.mana = j["mana"].get<Stat>();
}
