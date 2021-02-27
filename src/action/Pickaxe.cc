#include "Pickaxe.hh"


using json = nlohmann::json;
using namespace std;

/* Pickaxe constructor. */
Pickaxe::Pickaxe(ActionType type, string path) : Block(type, path) {
    /* Load the right json based on the type. */
    std::string filename = path + Item::getJsonFilename(type);
    std::ifstream infile(filename);
    /* Check that the file could be opened. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);

    blockDamage = j["blockDamage"];
    tier = j["tier"];
}

/* Pickaxe use. */
bool Pickaxe::use_internal(InputType type, int x, int y, World &world) {
    // Only do anything if the tile is within range
    if (!canPlace(x, y, world.player, world.map)) {
        return false;
    }

    /* Which layer to damage. */
    MapLayer layer = getLayer(type);
    Location place = world.map.getMapCoords(x, y, layer);
    /* Only mine blocks this pickaxe is capable of mining. */
    if (world.map.getTile(place) -> getTier() > tier) {
        return false;
    }

    bool success = world.map.damage(place, blockDamage, world.droppedItems);
    return success;
}
