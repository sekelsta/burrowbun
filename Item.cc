#include <string>
#include <fstream>
#include <vector>
#include "Item.hh"
#include "json.hpp"

/* For convenience. */
using json = nlohmann::json;

using namespace std;

// Constructor
Item::Item(ItemType type) {
    itemType = type;

    /* Figure out which json file to use. */
    string filename = getJsonFilename(type);

    /* Put the data in the json. */
    ifstream infile(filename);
    json j = json::parse(infile);

    /* Set each of the values equal to the json's values. */
    sprite = j["sprite"].get<Sprite>();
    isItem = j["isItem"];
    maxStack = j["maxStack"];
    useTime = j["useTime"];
}

// Destructor must be virtual
Item::~Item() {};

// Access functions
ItemType Item::getType() {
    return itemType;
}

/* Get json filename from itemtype. */
std::string Item::getJsonFilename(ItemType type) {
    string filename;
    string prefix = "items/";
    string suffix = ".json";

    /* Actually set filename to the right value. */
    switch(type) {
        case ItemType::DIRT:
            filename = "dirt";
            break;
        case ItemType::HUMUS:
            filename = "humus";
            break;
        case ItemType::CLAY:
            filename = "clay";
            break;
        case ItemType::CALCAREOUS_OOZE:
            filename = "calcareous_ooze";
            break;
        case ItemType::SNOW:
            filename = "snow";
            break;
        case ItemType::ICE:
            filename = "ice";
            break;
        case ItemType::STONE:
            filename = "stone";
            break;
        case ItemType::GRANITE:
            filename = "granite";
            break;
        case ItemType::BASALT:
            filename = "basalt";
            break;
        case ItemType::LIMESTONE:
            filename = "limestone";
            break;
        case ItemType::MUDSTONE:
            filename = "mudstone";
            break;
        case ItemType::PERIDOTITE:
            filename = "peridotite";
            break;
        case ItemType::SANDSTONE:
            filename = "sandstone";
            break;
        case ItemType::RED_SANDSTONE:
            filename = "red_sandstone";
            break;
        case ItemType::PLATFORM:
            filename = "platform";
            break;
        case ItemType::LUMBER:
            filename = "lumber";
            break;
        case ItemType::RED_BRICK:
            filename = "red_brick";
            break;
        case ItemType::GRAY_BRICK:
            filename = "gray_brick";
            break;
        case ItemType::DARK_BRICK:
            filename = "dark_brick";
            break;
        case ItemType::SAND:
            filename = "sand";
            break;
        case ItemType::MAPLE_LEAF:
            filename = "maple_leaf";
            break;
        case ItemType::HEALTH_POTION:
            filename = "health_potion";
            break;
        case ItemType::PICKAXE:
            filename = "pickaxe";
            break;
        default:
            assert(false);
            break;
    }

    return prefix + filename + suffix;
}
