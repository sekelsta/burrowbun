#include "WindowHandler.hh"

using namespace std;

int main(int argc, char **argv) {
    // Do the stuff it would be doing without the images
        Map m = Map(WorldType::EARTH);
        m.save("map.world");
        Map t = Map(WorldType::TEST);
        t.save("test.world");
    // Declare variables for rendering a window
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    // Construct a WindowHandler
    WindowHandler window(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Start SDL and open the window
    if (!window.init()) {
        exit(1);
    }

    // Load any pictures
    if (!window.loadMedia(m.getPointers())) {
        exit(1);
    }

    // Event handler
    SDL_Event e;

    // Loop infinitely until exiting
    bool quit = false;
    while (!quit) {
        // Handle events on the queue
        while(SDL_PollEvent(&e) != 0) {
            // Check whether to quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Put pictures on the screen
        window.update();
    }
    window.close();
    return 0;
}
