#include <SDL2/SDL.h>
#include <vector>
#include "Player.hh"
#include "Item.hh"
#include "AllTheItems.hh"
#include "Action.hh"
#include <iostream>
#include "json.hh"
#include "filepaths.hh"
#include "DroppedItem.hh"

using namespace std;
using json = nlohmann::json;

#define PLAYER_PICKUP_DISTANCE 128

// Constructor
Player::Player(string path) : Entity(path + "entities/bunny.json", path), 
        inventory(12, 5, path), trash(1, 1, path, true), hotbar(path) {
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
    inventory.x = hotbar.getX();
    // The position of the bottom of the hotbar
    inventory.y = hotbar.getY() + hotbar.getHeight();
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
    pickup(ItemMaker::makeItem(ActionType::PICKAXE, path));
    for (int i = 0; i < 20; i++) {
        pickup(ItemMaker::makeItem(ActionType::HEALTH_POTION, path));
        pickup(ItemMaker::makeItem(ActionType::GLOWSTONE, path));
        pickup(ItemMaker::makeItem(ActionType::GLASS, path));
    }
    pickup(ItemMaker::makeItem(ActionType::DIRT, path));
    pickup(ItemMaker::makeItem(ActionType::TOPSOIL, path));
    pickup(ItemMaker::makeItem(ActionType::SAND, path));
    pickup(ItemMaker::makeItem(ActionType::CLAY, path));
    pickup(ItemMaker::makeItem(ActionType::SNOW, path));
    pickup(ItemMaker::makeItem(ActionType::ICE, path));
    pickup(ItemMaker::makeItem(ActionType::STONE, path));
    pickup(ItemMaker::makeItem(ActionType::GRANITE, path));
    pickup(ItemMaker::makeItem(ActionType::BASALT, path));
    pickup(ItemMaker::makeItem(ActionType::PERIDOTITE, path));
    pickup(ItemMaker::makeItem(ActionType::SANDSTONE, path));
    pickup(ItemMaker::makeItem(ActionType::PLATFORM, path));
    pickup(ItemMaker::makeItem(ActionType::LUMBER, path));
    pickup(ItemMaker::makeItem(ActionType::RED_BRICK, path));
    pickup(ItemMaker::makeItem(ActionType::GRAY_BRICK, path));
    pickup(ItemMaker::makeItem(ActionType::DARK_BRICK, path));
    pickup(ItemMaker::makeItem(ActionType::GLOWSTONE, path));
    pickup(ItemMaker::makeItem(ActionType::GLASS, path));
    pickup(ItemMaker::makeItem(ActionType::TORCH, path));

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
            /* Assume the item was consumable and tell the inventory to 
            update. */
            inventory.touch();
        }
        /* That thing could have been in the hotbar, so better update its
        sprite. */
        hotbar.touch();
    }
}

void Player::update(vector<DroppedItem*> &drops) {
    Entity::update(drops);
    // Tick down the time until we can use items again
    assert(useTimeLeft >= 0);
    if (!canUse()) {
        useTimeLeft--;
    }
    /* Update the bars so changes to the stats will actually be displayed. */
    healthBar.update(health);
    fullnessBar.update(fullness);
    manaBar.update(mana);
    /* Delete empty stacks in inventories, so they don't interfere. */
    inventory.update();
    trash.update();
    hotbar.Inventory::update();
    // Update the inventories
    inventory.update(mouseSlot);
    trash.update(mouseSlot);
    hotbar.update(mouseSlot, isInventoryOpen);
    if (mouseSlot && mouseSlot -> isItem() 
            && ((Item *)mouseSlot) -> getStack() <= 0) {
        delete mouseSlot;
        mouseSlot = nullptr;
    }
    /* Drop the mouse item on the ground if the inventory is closed. */
    if (!isInventoryOpen) {
        toss(drops);
    }

    /* Again delete empty stacks in inventories, this time so they don't
    render weirldy. */
    inventory.update();
    trash.update();
    hotbar.Inventory::update();
}

Item *Player::pickup(Item *item) {
    item = hotbar.stack(item);
    item = inventory.stack(item);
    if (item && item->getStack()) {
        item = hotbar.pickup(item);
        item = inventory.pickup(item);
    }
    else {
        delete item;
        item = nullptr;
    }
    return item;
}

void Player::pickup(DroppedItem *item) {
    /* Ignore items that have just been thrown. */
    if (!item -> canPickup()) {
        return;
    }

    // Pick it up if colliding with it
    if (rect.intersects(item -> getRect())) {
        item -> item = pickup(item -> item);
        return;
    }
    // TODO: remove magic number
    attractOther(PLAYER_PICKUP_DISTANCE, ITEM_ATTRACT_SPEED, item);
}

DroppedItem *Player::drop() {
    if (mouseSlot && mouseSlot -> isItem()) {
        DroppedItem *dropped = new DroppedItem((Item *)mouseSlot, 
            getCenterX(), getCenterY(), rect.worldWidth);
        dropped -> toss(isFacingRight, PLAYER_PICKUP_DISTANCE);
        mouseSlot = nullptr;
        return dropped;
    }
    return nullptr;
}





