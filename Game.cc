#include "Game.hh"

#include <iostream>
#include <cassert>
#include "Tile.hh"
#include "Map.hh"
#include "Mapgen.hh"
#include "EventHandler.hh"
#include "Movable.hh"
#include "Entity.hh"
#include "Player.hh"
#include "Collider.hh"
#include "Hotbar.hh"

using namespace std;

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

/* Create a new world. */
void Game::createWorld(string filename) {
    Mapgen mapgen;
    mapgen.generate(filename, WorldType::EARTH);
}

void Game::play(string mapname) {
    /* Load a map. */
    Map map = Map(mapname, TILE_WIDTH, TILE_HEIGHT);
    window.setMapSize(map.getWidth(), map.getHeight());

    SDL_Event event;
    EventHandler eventHandler;

    Player player;

    /* Vectors to hold all the things that need to collide. */
    vector<DroppedItem *> droppedItems;
    vector<movable::Movable *> movables;
    movables.push_back(&player);
    /* Set the player's position to the spawnpoint. */
    player.setX(map.getSpawn().x * TILE_WIDTH);
    player.setY(map.getSpawn().y * TILE_HEIGHT);

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
        collider.update(map, movables, droppedItems);
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
    map.save(mapname);
}

Game::Game() : SCREEN_FPS(60), TICKS_PER_FRAME(1000 / SCREEN_FPS),
    // 800 x 600 window, resizable
    window(800, 600, TILE_WIDTH, TILE_HEIGHT) {
}

void Game::run() {
    const string filename = "world.world";
    createWorld(filename);

    play(filename);
}

