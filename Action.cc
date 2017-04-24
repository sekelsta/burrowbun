#include <iostream>
#include "Action.hh"
#include "Inventory.hh"

// Constructor
Action::Action() {
    sprite.width = Inventory::squareSprite.width;
    sprite.height = Inventory::squareSprite.height;
    useTime = 1;
}

/* Do the action, or use the item or skill. */
void Action::use(InputType type, int x, int y, Player &player, Map &map) {
    // Pass
}

// Access functino
int Action::getUseTime() const {
    return useTime;
}
