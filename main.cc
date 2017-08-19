#include <iostream>
#include <cassert>
#include "Tile.hh"
#include "Map.hh"
#include "Mapgen.hh"
#include "WindowHandler.hh"
#include "EventHandler.hh"
#include "Movable.hh"
#include "Entity.hh"
#include "Player.hh"
#include "Collider.hh"
#include "Hotbar.hh"

/* TODO: make unneeded and remove. */
#define TILE_WIDTH 16
#define TILE_HEIGHT 16

using namespace std;

/* For capping the frame rate. */
const int SCREEN_FPS = 60;
const int TICKS_PER_FRAME = 1000 / SCREEN_FPS;

/* Define the static class member. */
Sprite Inventory::squareSprite;

int main(int argc, char **argv) {
    /* Declare variables for rendering a window. */
    int screenWidth = 800;
    int screenHeight = 600;

    /* Create and start a world. */
    Mapgen mapgen;
    mapgen.generate("world.world", WorldType::SMOLTEST);
    Map map = Map("world.world", TILE_WIDTH, TILE_HEIGHT);

    /* Construct a WindowHandler. */
    bool enableDarkness = true;
    WindowHandler window(screenWidth, screenHeight, map.getWidth(), 
                    map.getHeight(), TILE_WIDTH, TILE_HEIGHT, enableDarkness);

    /* Start SDL and open the window. */
    if (!window.init()) {
        exit(1);
    }

    SDL_Event event;
    EventHandler eventHandler;

    Player player;

    /* A vector to hold all the things that need to collide. */
    vector<movable::Movable *> movables;
    movables.push_back(&player);
    /* Set the player's position to the spawnpoint. */
    player.x = map.getSpawn().x * TILE_WIDTH;
    player.y = map.getSpawn().y * TILE_HEIGHT;

    /* Load any pictures. */
    if (!window.loadMedia(map.getPointersRef(), movables, player.hotbar)) {
        exit(1);
    }

    Collider collider(TILE_WIDTH, TILE_HEIGHT);

    /* Whether the window is in focus. */
    bool isFocused = true;

    /* Frames since the start of the game. */
    uint32_t gameTicks = 0;

    /* Loop infinitely until exiting. */
    bool quit = false;
    while (!quit) {
        Uint32 ticks = SDL_GetTicks();
        /* Handle events on the queue. */
        while(SDL_PollEvent(&event) != 0) {
            /* Check whether to quit. */
            switch(event.type) {
                case SDL_QUIT: 
                    quit = true;
                    break;
                /* Pass the event and relevent information to EventHandler. */
                case SDL_WINDOWEVENT:
                    eventHandler.windowEvent(event, isFocused, window);
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    eventHandler.keyEvent(event, player);
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEWHEEL:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    eventHandler.mouseEvent(event);
                    break;
                default:
                    // Pass
                    break;
            }
        }

        /* TODO: move elsewhere, and update all entities. */
        player.update();

        /* Now that all the events have been handled, do eventhandling things 
        that need to be done every update (like checking whether any keys
        or mouse buttons are being held down). */
        eventHandler.update(player, map);

        /* Move things around. */
        collider.update(map, movables);
        /* Have every movable take fall damage. */
        for (unsigned int i = 0; i < movables.size(); i++) {
            movables[i] -> takeFallDamage();
        }

        /* Have the map update itself and relevent movables. */
        map.update(movables);

        /* Put pictures on the screen, but only if rendering isn't really 
        slow. */
        uint32_t frameTicks = SDL_GetTicks() - ticks;
        if (frameTicks < 2 * TICKS_PER_FRAME) { 
            window.update(map, movables, player);
        }

        /* Count the number of times we've gone through this loop. */
        gameTicks++;

        /* Wait for enough time to pass before doing the next frame. */
        frameTicks = SDL_GetTicks() - ticks;
        if (frameTicks < TICKS_PER_FRAME) {
            SDL_Delay(TICKS_PER_FRAME - frameTicks);
        }
    }
    window.close();
    return 0;
}


