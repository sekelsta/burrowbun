#include "World.hh"

using namespace std;

World::World(string filename, int tileWidth, int tileHeight, string path) 
        : map(filename, tileWidth, tileHeight, path), player(path),
        collider(tileWidth, tileHeight) {
    
    movables.push_back(&player);
    /* Set the player's position to the spawnpoint. */
    player.setX(map.getSpawn().x * tileWidth);
    player.setY(map.getSpawn().y * tileHeight);
}

World::~World() {
    while (!droppedItems.empty()) {
        delete droppedItems.back();
        droppedItems.erase(droppedItems.end() - 1);
    }
}

void World::update() {

    /* TODO: update all entities. */
    player.update();


    /* Move things around. */
    collider.update(map, movables, droppedItems);
    /* Have every movable take fall damage. */
    for (unsigned int i = 0; i < movables.size(); i++) {
        movables[i] -> takeFallDamage();
    }

    /* Have the map update itself and relevent movables. */
    map.update(movables);

}
