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

/* Deal damage to whatever is overlapping this, and stop it if this tile is 
solid. */
void Tile::dealOverlapDamage(movable::Movable &movable) const {
    movable.takeDamage(overlapDamage);
}

Location Tile::getSpritePlace(Map &map, const Location &place) const {
    Location answer;
    /* Some (empty) tiles have no sprite. */
    if (!sprite.hasTexture()) {
        answer.x = 0;
        answer.y = 0;
        return answer;
    }
    answer.y = map.bordering(place);
    answer.x = rand() % numSprites();
    /* On the sprite, the equivalent background tile is moved over by
    sprite.cols / 2. */
    if (place.layer == MapLayer::BACKGROUND) {
        assert(canBackground);
        answer.x += sprite.getCols() / 2;
    }

    return answer;
}

Location Tile::updateSprite(Map &map, const Location &place) const {
    Location answer;
    answer.y = map.bordering(place);
    answer.x = map.getSprite(place).x;
    return answer;
}

/* Change the map in whatever way needs doing. */
bool Tile::update(Map &map, Location place,
        std::vector<DroppedItem*> &items, int tick) const {
    if (isAnimated) {
        if (tick % TILE_ANIMATION_DELAY == 0) {
            Location spritePlace = map.getSprite(place);
            spritePlace.x = (spritePlace.x + 1) % numSprites();
            map.setSprite(place, spritePlace);
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

void Tile::render(uint8_t spritePlace, const Light &light, 
        const SDL_Rect &rectTo) {
    if (!sprite.hasTexture()) {
        return;
    }

    /* Use darkness. */
    sprite.setColorMod(light);
    Location spriteLocation;
    SpaceInfo::fromSpritePlace(spriteLocation, spritePlace);
    assert(spriteLocation.x >= 0);
    assert(spriteLocation.y >= 0);
    assert(sprite.getWidth() > 0);
    assert(sprite.getHeight() > 0);
    sprite.move(spriteLocation.x * sprite.getWidth(), 
            spriteLocation.y * sprite.getHeight());
    sprite.render(rectTo);
}
