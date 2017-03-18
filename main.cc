#include <iostream>
#include "Tile.hh"
#include "Map.hh"
#include "World.hh"
#include "WindowHandler.hh"
#include "EventHandler.hh"
#include "Movable.hh"
#include "Player.hh"
#include "Collider.hh"
#include "Hotbar.hh"

using namespace std;

// for capping frame rate
const int SCREEN_FPS = 60;
const int TICKS_PER_FRAME = 1000 / SCREEN_FPS;

int main(int argc, char **argv) {
    // Do the stuff it would be doing without the images
    World world = World(WorldType::EARTH);
    world.save("world.world");
    Map map = Map("world.world");
    map.save("map.bmp");

    // Declare variables for rendering a window
    int screenWidth = 800;
    int screenHeight = 600;

    const int TILE_WIDTH = 16;
    const int TILE_HEIGHT = 16;

    // Construct a WindowHandler
    bool enableDarkness = true;
    WindowHandler window(screenWidth, screenHeight, map.getWidth(), 
                    map.getHeight(), TILE_WIDTH, TILE_HEIGHT, enableDarkness);

    // Start SDL and open the window
    if (!window.init()) {
        exit(1);
    }

    // Event and event handler
    SDL_Event event;
    EventHandler eventHandler;

    // Player
    Player player;
    assert(player.hotbar.smallGap == 4);

    // A vector to hold all the things that need to collide
    vector<Movable *> movables;
    movables.push_back(&player);
    // Set the player's position to the spawnpoint
    player.x = map.getSpawn().x * TILE_WIDTH;
    player.y = map.getSpawn().y * TILE_HEIGHT;

    // Load any pictures
    if (!window.loadMedia(map.getPointersRef(), movables, player.hotbar)) {
        exit(1);
    }

    // Collision handler
    Collider collider(TILE_WIDTH, TILE_HEIGHT);

    // Variable to tell whether the window is in focus
    bool isFocused = true;

    // Frames since the start of the game
    Uint32 gameTicks = 0;

    // Loop infinitely until exiting
    bool quit = false;
    while (!quit) {
        Uint32 ticks = SDL_GetTicks();
        // Handle events on the queue
        while(SDL_PollEvent(&event) != 0) {
            // Check whether to quit
            switch(event.type) {
                case SDL_QUIT: 
                    quit = true;
                    break;
                // Pass the event and relevent information to EventHandler
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
                    eventHandler.mouseEvent(event, player);
                    break;
                default:
                    // TODO
                    // cerr << "Received unsupported event." << endl;
                    break;
            }
        }

        // Now that all the events have been handled, look at which keys are
        // currently being held down and handle those too
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        eventHandler.updateKeys(state);

        // Move things around
        eventHandler.updatePlayer(player);
        collider.update(map, movables);

        // Put pictures on the screen
        // But only if rendering isn't really slow
        Uint32 frameTicks = SDL_GetTicks() - ticks;
        if (frameTicks < 2 * TICKS_PER_FRAME) { 
            window.update(map, movables, player.hotbar);
        }

        // Count the number of times we've gone through this loop
        gameTicks++;

        // Wait for enough time to pass before doing the next frame
        frameTicks = SDL_GetTicks() - ticks;
        if (frameTicks < TICKS_PER_FRAME) {
            SDL_Delay(TICKS_PER_FRAME - frameTicks);
        }
    }
    window.close();
    return 0;
}


