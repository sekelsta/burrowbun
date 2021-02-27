#ifndef GAME_HH
#define GAME_HH

#include <string>
#include "render/WindowHandler.hh"
#include "EventHandler.hh"
#include "world/MapHelpers.hh"

class Menu;

class Game { 
    /* For capping the frame rate. */
    const uint32_t SCREEN_FPS;
    const uint32_t TICKS_PER_FRAME;

    /* The path to the folder containing the executable. */
    static std::string path;

    EventHandler eventHandler;
    WindowHandler window;

    /* Whether the window is in focus. */
    bool isFocused;

    /* Whether we're currently playing or at a menu screen. */
    bool isPlaying;

    /* Things we might have. */
    Menu *menu;
    World *world;

    /* Load the given map and start playing. */
    bool play(std::string mapname);

    /* Poll the event queue and update internal state. Return true if the
    user requested quit. */
    bool update();
public:
    /* Constructor. Takes the path to the folder containing the executable. */
    Game(std::string p);

    /* Get the path. */
    static inline std::string getPath() {
        return path;
    }

    /* Do everything. */
    void run();
};

#endif
