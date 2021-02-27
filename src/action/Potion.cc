#include "Potion.hh"
#include "../json.hh"

using json = nlohmann::json;
using namespace std;

// Potion constructor
Potion::Potion(ActionType type, string path) : Item(type, path) {
    /* Figure out which json file to use. */
    std::string filename = path + Item::getJsonFilename(type);

    /* Put the data into the json. */
    std::ifstream infile(filename);
    /* Check that the file could be opened. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);

    /* Set values equal to the json's values. */
    healthGained = j["healthGained"];
    fullnessGained = j["fullnessGained"];
    manaGained = j["manaGained"];
    woundsCured = j["woundsCured"];
    hungerCured = j["hungerCured"];
    manaCured = j["manaCured"];
}

// Add the potion amount to all the stats
bool Potion::use_internal(InputType type, int x, int y, World &world) {
    // Add the potion amount to the player, but only if the left mouse
    // button wqs pressed (not held, and not the right button).
    if (type == InputType::LEFT_BUTTON_PRESSED) {
        world.player.health.addPart(woundsCured);
        world.player.fullness.addPart(hungerCured);
        world.player.mana.addPart(manaCured);
        world.player.health.addFull(healthGained);
        world.player.fullness.addFull(fullnessGained);
        world.player.mana.addFull(manaGained);
        return true;
    }
    return false;
}

