#include "Block.hh"
#include "ItemMaker.hh"

using json = nlohmann::json;
using namespace std;

// Block constructor
Block::Block(ActionType type, string path) : Item(type, path) {
    /* Make sure we should actually be a block. */
    assert(ActionType::FIRST_BLOCK <= type);
    assert(type <= ActionType::LAST_BLOCK);

    /* Read in the json. */
    std::string filename = path + Item::getJsonFilename(type);
    std::ifstream infile(filename);
    /* Check that the file could be opened. */
    if (!infile) {
        cerr << "Can't open " << filename << "\n";
    }
    json j = json::parse(infile);

    /* Set values. */
    bonusReach = j["bonusReach"];
    
    /* (The json doesn't include the tiletype) */
    if (type <= ActionType::LAST_PURE_BLOCK) {
        tileType = ItemMaker::itemToTile(type);
    }
    else {
        tileType = TileType::EMPTY;
    }
}

/* Destructor must be virtual. */
Block::~Block() {};

/* Tell whether the player can reach far enough to place the block here. */
bool Block::canPlace(int x, int y, const Player &player, const Map &map) {
    // Figure out which tile the mouse is over
    int xTile = x / map.getTileWidth();
    int yTile = y / map.getTileHeight();

    // Figure out which tile the top middle of the player is at, so we can
    // see whether the tile is in range
    int xPlayer = (player.getRect().x + (player.getWidth() / 2));
    xPlayer /= map.getTileWidth();
    int yPlayer = (player.getRect().y + player.getHeight());
    yPlayer /= map.getTileHeight();

    /* And now we have our answer. We don't need to do anything special about 
    wrapping the map because xTile will already be outside of the map range if 
    that's needed to get it numerically closer to the player. */
    return player.canReach(xTile - xPlayer, yTile - yPlayer, bonusReach); 
}

MapLayer Block::getLayer(InputType type) {
        /* Which layer to damage. */
        MapLayer layer = MapLayer::NONE;
        // If it was a left mouse button, place the tile in the foreground
        if (type == InputType::LEFT_BUTTON_PRESSED
                || type == InputType::LEFT_BUTTON_HELD) {
            layer = MapLayer::FOREGROUND;
        }
        // Otherwise, if it was the right mouse button, put the tile in the
        // background
        else if (type == InputType::RIGHT_BUTTON_PRESSED
                || type == InputType::RIGHT_BUTTON_HELD) {
            layer = MapLayer::BACKGROUND;
        }
        return layer; 
}

// When used, place the tile
bool Block::use_internal(InputType type, int x, int y, World &world) {
    // Only do anything if the tile is within range
    if (!canPlace(x, y, world.player, world.map)) {
        return false;
    }

    MapLayer layer = getLayer(type);

    /* Only do anything if it's a real layer. */
    if (layer == MapLayer::NONE) {
        return false;
    }

    /* If success is still false at the end, don't set the player's use
    time left. */
    bool success = world.map.placeTile(
            world.map.getMapCoords(x, y, layer), tileType);

    return success;
}
