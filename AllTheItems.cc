#include "AllTheItems.hh"
#include "Player.hh"

// Potion constructor
Potion::Potion(ItemType type) : Item(type) {
    // Set values to defaults
    healthGained = 0;
    staminaGained = 0;
    manaGained = 0;
    woundsCured = 0;
    hungerCured = 0;
    manaCured = 0;
 
    // Changed some values based on what type of potion
    switch (type) {
        case ItemType::HEALTH_POTION :
            healthGained = 50;
            break;
    }
}

void Potion::use(InputType type, Player &player, Map &map) {
    player.health.addPart(woundsCured);
    player.stamina.addPart(hungerCured);
    player.mana.addPart(manaCured);
    player.health.addFull(healthGained);
    player.stamina.addFull(staminaGained);
    player.mana.addFull(manaGained);
}


// Whether the type is in the vector
bool ItemMaker::isIn(std::vector<ItemType> items, ItemType type) {
    for (unsigned int i = 0; i < items.size(); i++) {
        if (items[i] == type) {
            return true;
        }
    }
    // We've gone through the whole thing and it's not in there
    return false;
}

// Take an item type and make the correct child class based on that
Item *ItemMaker::makeItem(ItemType type) {
    std::vector<ItemType> potions;
    potions.push_back(ItemType::HEALTH_POTION);

    // If it's a potion, make a potion
    if (isIn(potions, type)) {
        return new Potion(type);
    }
    // If it's not a subclass of item, than it's a plain old item
    else {
        return new Item(type);
    }
}

