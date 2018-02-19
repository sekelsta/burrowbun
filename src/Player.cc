#include <SDL2/SDL.h>
#include <vector>
#include "Player.hh"
#include "Item.hh"
#include "AllTheItems.hh"
#include "Action.hh"
#include <iostream>
#include "json.hpp"
#include "filepaths.hh"
#include "DroppedItem.hh"

using namespace std;
using json = nlohmann::json;

// Constructor
Player::Player(string path) : Entity(path + "entities/bunny.json", path), 
        inventory(10, 6, path), trash(1, 1, path), hotbar(path) {
    hasInventory = true;

    /* Open json file that contains info about the stat bars. */
    ifstream bar_infile(path + "UI/stat_bars.json");
    if (!bar_infile) {
        cerr << "Can't open " << path + "UI/stat_bars.json" << "\n";
    }
    json jstats = json::parse(bar_infile);

    healthBar = jstats["healthBar"].get<StatBar>();
    fullnessBar = jstats["fullnessBar"].get<StatBar>();
    manaBar = jstats["manaBar"].get<StatBar>();

    // Make sure the bars start with the correct stats
    healthBar.update(health);
    fullnessBar.update(fullness);
    manaBar.update(mana);

    /* Load spritebar textures. */
    healthBar.overlay.loadTexture(path + UI_SPRITE_PATH);
    fullnessBar.overlay.loadTexture(path + UI_SPRITE_PATH);
    manaBar.overlay.loadTexture(path + UI_SPRITE_PATH);

    ifstream infile(path + "entities/bunny.json");
    if (!infile) {
        cerr << "Can't open " << path + "entities/bunny.json" << "!\n";
    }
    json j = json::parse(infile);

    tileReachUp = j["tileReachUp"];
    tileReachDown = j["tileReachDown"];
    tileReachSideways = j["tileReachSideways"];

    // Assume it hasn't used an item
    useTimeLeft = 0;

    // We don't need to set the statbar colors because the entity constructor
    // already did.

    // Set the location of the inventory
    inventory.x = hotbar.xStart;
    // The position of the bottom of the hotbar
    inventory.y = hotbar.yStart + hotbar.frame.getHeight() + hotbar.offsetDown;
    // The size of the gap between the hotbar and the inventory
    inventory.y += 16;
    trash.x = inventory.x;
    // The 32s here are for Inventory::squareSprite.width and height,
    // which haven't been initialized yet. TODO: fix
    trash.x += (inventory.getWidth() - 1) * 32;
    trash.y = inventory.y + 4;
    trash.y += inventory.getHeight() * 32;

    // Have them update where their clickboxes are
    inventory.updateClickBoxes();
    trash.updateClickBoxes();

    // Start with nothing in the mouse slot
    mouseSlot = NULL;

    // Have starting items
    inventory.pickup(ItemMaker::makeItem(ItemType::HEALTH_POTION, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::DIRT, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::TOPSOIL, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::SAND, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::CLAY, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::CALCAREOUS_OOZE, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::SNOW, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::ICE, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::STONE, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::GRANITE, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::BASALT, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::LIMESTONE, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::MUDSTONE, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::PERIDOTITE, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::SANDSTONE, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::RED_SANDSTONE, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::PLATFORM, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::LUMBER, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::RED_BRICK, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::GRAY_BRICK, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::DARK_BRICK, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::MUD, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::CLOUD, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::BOULDER, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::GLACIER, path));
    inventory.pickup(ItemMaker::makeItem(ItemType::PICKAXE, path));

    isInventoryOpen = false;
}

// Switch whether the inventory is open or closed
void Player::toggleInventory() {
    isInventoryOpen = !isInventoryOpen;
}

// Whether a place is within range for placing tiles
// x is distance from the player horizontally, in tiles.
// y is distance from the player in tiles, with positive being above the 
// player. bonus is the bonus range from possible unknown curcumstances
// (e.g. this type of tile can be placed farther away, or this pickax has
// better range).
bool Player::canReach(int x, int y, int bonus) const {
    // If not in range in the x direction, return false
    if (abs(x) > tileReachSideways + bonus) {
        return false;
    }

    // If too high, return false
    if (y > tileReachUp + bonus) {
        return false;
    }

    // If too low, return false
    if (-1 * y > tileReachDown + bonus) {
        return false;
    }

    // Otherwise, it's within reach
    return true;

}

// Whether the player can use an item or skill right now
bool Player::canUse() {
    return useTimeLeft == 0;
}

// Use the item or skill held or selected
void Player::useAction(InputType type, int x, int y, World &world) {
    // Only bother if we actually can
    if (canUse() && type != InputType::NONE) {
        // Try to use the item held by the mouse
        if (mouseSlot != NULL) {
            mouseSlot -> use(type, x, y, world);
        }
        // Try to use the item in the selected hotbar slot
        else if (hotbar.getSelected() != NULL) {
            hotbar.getSelected() -> use(type, x, y, world);
        }
    }
}

void Player::update() {
    Entity::update();
    /* Update the bars so changes to the stats will actually be displayed. */
    healthBar.update(health);
    fullnessBar.update(fullness);
    manaBar.update(mana);
}

void Player::pickup(DroppedItem *item) {
    // Pick it up if colliding with it
    if (rect.intersects(item -> getRect())) {
        item -> item = inventory.pickup(item -> item);
    }
}








