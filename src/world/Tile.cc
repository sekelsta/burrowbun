#include "Tile.hh"
#include "Map.hh"
#include "../entity/DroppedItem.hh"
#include "../entity/Movable.hh"
#include "../filepaths.hh"
#include "../util/PathToExecutable.hh"

#include <nlohmann/json.hpp>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <SDL2/SDL.h>

#define TILE_ANIMATION_DELAY 4

// For convenience
using json = nlohmann::json;

using namespace std;

/* Get the filename of the json for this tiletype. */
std::string Tile::getFilename() {
    string prefix = "tiles/";
    string suffix = ".json";
    return PATH_TO_EXECUTABLE + prefix + name + suffix;
}

// All the access functions
bool Tile::getIsPlatform() const {
    return isPlatform;
}

bool Tile::getIsSolid() const {
    return isSolid;
}

int Tile::getMaxHealth() const {
    return maxHealth;
}

uint8_t Tile::getInitialVariant() const {
    return uint8_t(rand() % getNumVariants());
}

/* Deal damage to whatever is overlapping this, and stop it if this tile is 
solid. */
void Tile::dealOverlapDamage(movable::Movable &movable) const {
    movable.takeDamage(overlapDamage);
}

/* Change the map in whatever way needs doing. */
bool Tile::update(Map &map, Location place,
        std::vector<DroppedItem*> &items, int tick) const {
    if (isAnimated) {
        if (tick % TILE_ANIMATION_DELAY == 0) {
            uint8_t variant = map.getVariant(place.x, place.y, place.layer);
            map.setVariant(place.x, place.y, place.layer, (variant + 1) % numVariants);
        }
        return true;
    }
    return false;
}

// Constructor, based on the tile type
Tile::Tile(TileType tileType, string name_in) 
        : type(tileType), name(name_in) {
    /* Put data in json. */
    string filename = getFilename();
    ifstream infile(filename);
    /* Check that file was opened successfully. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);
    /* Set each of this tile's non-const values equal to the json's values. */
    sprite = j["sprite"].get<Sprite>();
    numVariants = j["numVariants"];
    color = j["color"].get<Light>();
    isSolid = j["isSolid"];
    isPlatform = j["isPlatform"];
    canBackground = j["canBackground"];
    waterBreaks = j["waterBreaks"];
    overlapDamage = j["overlapDamage"].get<Damage>();
    isAnimated = j["isAnimated"];
    emitted = j["emitted"].get<Light>();
    absorbed = {j["absorbed"]["r"], j["absorbed"]["g"], 
        j["absorbed"]["b"], j["absorbed"]["a"]};
    isSky = j["isSky"];
    maxHealth = j["maxHealth"];
    tier = j["tier"];
    int edgeInt = j["edgeType"];
    edgeType = (EdgeType)edgeInt;
    sprite.loadTexture(PATH_TO_EXECUTABLE + TILE_SPRITE_PATH);

    assert(absorbed.r >= 1.0);
    assert(absorbed.g >= 1.0);
    assert(absorbed.b >= 1.0);
    /* Not technically required but I can imagine mistakenly making it so much
    more easily than I can imagine actually wanting it to be the case. */
    assert(absorbed.a < 1.0);
}

/* Virtual destructor. */
Tile::~Tile() {}

/* Whether the tile will ever need to call its update function. */
bool Tile::canUpdate(const Map &map, const Location &place) const {
    return isAnimated;
}

void Tile::render(uint8_t variant, uint8_t bordering, const Light &light, 
        const SDL_Rect &rectTo) {
    if (!sprite.hasTexture()) {
        return;
    }

    /* Use darkness. */
    sprite.setColorMod(light);
    assert(sprite.getWidth() > 0);
    assert(sprite.getHeight() > 0);
    sprite.move(variant * sprite.getWidth(), bordering * sprite.getHeight());
    sprite.render(rectTo);
}
