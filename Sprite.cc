#include <fstream>
#include "Sprite.hh"
#include "json.hpp"

/* For convinence. */
using json = nlohmann::json;
using namespace std;

// Constructor
Sprite::Sprite() {
    // Initialize all the values
    name = "";
    texture = NULL;
    width = 0;
    height = 0;
    rows = 0;
    cols = 0;
    row = 0;
    col = 0;
}

/* Assignment operator. */
Sprite &Sprite::operator=(const Sprite &sprite) {
    /* Check for self-assignment. */
    if (this == &sprite) {
        return *this;
    }
    name = sprite.name;
    texture = sprite.texture;
    width = sprite.width;
    height = sprite.height;
    rows = sprite.rows;
    cols = sprite.cols;
    row = sprite.row;
    col = sprite.col;

    return *this;
}

/* Get a sprite out of a json. */
void from_json(const json &j, Sprite &sprite) {
    /* Set each of this tile's non-const values equal to the json's values. */
    sprite.name = j["name"];
    sprite.rows = j["rows"];
    sprite.cols = j["cols"];
    sprite.row = j["row"];
    sprite.col = j["col"];
    sprite.width = j["width"];
    sprite.height = j["height"];
    sprite.texture = NULL; 
}

