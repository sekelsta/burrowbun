#include "Game.hh"

#include <iostream>
#include <cassert>
#include "Tile.hh"
#include "Mapgen.hh"
#include "EventHandler.hh"
#include "Movable.hh"
#include "Entity.hh"
#include "Hotbar.hh"

#include "World.hh"

using namespace std;

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

/* Create a new world. */
void Game::createWorld(string filename, WorldType type) {
    Mapgen mapgen(path);
    mapgen.generate(path + filename, type, path);
}

void Game::play(string mapname) {
    /* Load a world. */
    World world = World(path + mapname, TILE_WIDTH, TILE_HEIGHT, path);

    window.setMapSize(world.map.getWidth(), world.map.getHeight());

    SDL_Event event;
    EventHandler eventHandler;

    /* Whether the window is in focus. */
    bool isFocused = true;

    /* Frames since the start of the world. */
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
                    eventHandler.keyEvent(event, world.player);
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

        /* Now that all the events have been handled, do eventhandling things 
        that need to be done every update (like checking whether any keys
        or mouse buttons are being held down). */
        eventHandler.update(world);

        world.update();

        /* Put pictures on the screen, but only if rendering isn't really 
        slow. */
        uint32_t frameTicks = SDL_GetTicks() - ticks;
        if (frameTicks < 2 * TICKS_PER_FRAME) { 
            window.update(world);
        }

        /* Count the number of times we've gone through this loop. */
        gameTicks++;

        /* Wait for enough time to pass before doing the next frame. */
        frameTicks = SDL_GetTicks() - ticks;
        if (frameTicks < TICKS_PER_FRAME) {
            SDL_Delay(TICKS_PER_FRAME - frameTicks);
        }
    }
    world.map.save(path + mapname);
}

Game::Game(string p) : SCREEN_FPS(60), TICKS_PER_FRAME(1000 / SCREEN_FPS),
        // 800 x 600 window, resizable
        window(800, 600, TILE_WIDTH, TILE_HEIGHT) {
    path = p;
}

void Game::run() {
    const string filename = "world.world";
    createWorld(filename, WorldType::TEST);

    play(filename);
}

