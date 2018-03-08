#include <fstream>
#include <vector>
#include "Item.hh"
#include "json.hpp"
#include "filepaths.hh"
#include "AllTheItems.hh"
#include "Game.hh"
#include <SDL2/SDL.h>
#include <algorithm>

/* For convenience. */
using json = nlohmann::json;

using namespace std;

bool Item::use_internal(InputType type, int x, int y, World &world) {
    return false;
}

// Constructor
Item::Item(ActionType t, string path) {
    type = t;
    item = true;

    /* Figure out which json file to use. */
    string filename = path + getJsonFilename(type);

    /* Put the data in the json. */
    ifstream infile(filename);
    /* Check that file was opened successfully. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);

    /* Set each of the values equal to the json's values. */
    sprite = j["sprite"].get<Sprite>();
    maxStack = j["maxStack"];
    useTime = j["useTime"];
    consumable = j["consumable"];
    stack = 1;
    sprite.loadTexture(path + ICON_SPRITE_PATH);
}

void Item::use(InputType type, int x, int y, World &world) {
    assert(stack > 0);
    stack -= (int)isConsumable() * (int)use_internal(type, x, y, world);
}

Item::~Item() {}

void Item::render(SDL_Rect &rect, std::string path) {
    int w = sprite.getWidth();
    int h = sprite.getHeight();
    /* Center inside given rect. */
    SDL_Rect rectTo = {rect.x + (rect.w - w) / 2, rect.y + (rect.h - h) / 2,
        w, h};
    /* Render sprite. */
    sprite.render(rectTo);
    /* Render text only if there's more than one in the stack. */
    if (getStack() != 1) {
        Texture num(to_string(getStack()), ITEMSTACK_FONT_SIZE, 0);
        num.render(rect.x + ITEMSTACK_FONT_BUFFER_X, 
            rect.y - num.getHeight() + rect.h - ITEMSTACK_FONT_BUFFER_Y);
    }
}

Item *Item::merge(Item *other, int n) {
    assert(other || n <= 0);
    if (!n && !other) {
        return nullptr;
    }
    if (!other) {
        other = ItemMaker::makeItem(getType(), Game::getPath());
        other->setStack(0);
    }
    /* If they're different types, no merging can be done. */
    if (other -> getType() != getType()) {
        return other;
    }
    assert(maxStack == other->maxStack);
    /* Merge in the other direction. */
    if (n < 0) {
        n = min(-1 * n, getStack());
        n = min(n, maxStack - other->getStack());
        assert(n >= 0);
        other->setStack(other->getStack() + n);
        stack -= n;
        assert(stack >= 0);
        return other;
    }
    /* If n is 0, merge the maximum possible amount. */
    if (n == 0) {
        n = maxStack - getStack();
    }
    assert(n >= 0);
    n = min(n, other -> getStack());
    stack = stack + n;
    other -> setStack(other -> getStack() - n);
    return other;
}

/* Get json filename from ActionType. */
std::string Item::getJsonFilename(ActionType type) {
    string filename;
    string prefix = "items/";
    string suffix = ".json";

    /* Actually set filename to the right value. */
    switch(type) {
        case ActionType::DIRT:
            filename = "dirt";
            break;
        case ActionType::TOPSOIL:
            filename = "topsoil";
            break;
        case ActionType::CLAY:
            filename = "clay";
            break;
        case ActionType::CALCAREOUS_OOZE:
            filename = "calcareous_ooze";
            break;
        case ActionType::SNOW:
            filename = "snow";
            break;
        case ActionType::ICE:
            filename = "ice";
            break;
        case ActionType::STONE:
            filename = "stone";
            break;
        case ActionType::GRANITE:
            filename = "granite";
            break;
        case ActionType::BASALT:
            filename = "basalt";
            break;
        case ActionType::LIMESTONE:
            filename = "limestone";
            break;
        case ActionType::MUDSTONE:
            filename = "mudstone";
            break;
        case ActionType::PERIDOTITE:
            filename = "peridotite";
            break;
        case ActionType::SANDSTONE:
            filename = "sandstone";
            break;
        case ActionType::RED_SANDSTONE:
            filename = "red_sandstone";
            break;
        case ActionType::PLATFORM:
            filename = "platform";
            break;
        case ActionType::LUMBER:
            filename = "lumber";
            break;
        case ActionType::RED_BRICK:
            filename = "red_brick";
            break;
        case ActionType::GRAY_BRICK:
            filename = "gray_brick";
            break;
        case ActionType::DARK_BRICK:
            filename = "dark_brick";
            break;
        case ActionType::SAND:
            filename = "sand";
            break;
        case ActionType::MUD:
            filename = "mud";
            break;
        case ActionType::CLOUD:
            filename = "cloud";
            break;
        case ActionType::BOULDER:
            filename = "boulder";
            break;
        case ActionType::GLACIER:
            filename = "glacier";
            break;
        case ActionType::MAPLE_LEAF:
            filename = "maple_leaf";
            break;
        case ActionType::HEALTH_POTION:
            filename = "health_potion";
            break;
        case ActionType::PICKAXE:
            filename = "pickaxe";
            break;
        default:
            assert(false);
            break;
    }

    return prefix + filename + suffix;
}
