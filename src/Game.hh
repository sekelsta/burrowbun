#ifndef GAME_HH
#define GAME_HH

#include <string>
#include "WindowHandler.hh"

class Game { 
    /* For capping the frame rate. */
    const uint32_t SCREEN_FPS;
    const uint32_t TICKS_PER_FRAME;

    /* The path to the folder containing the executable. */
    std::string path;

    WindowHandler window;

    /* Create a new world. */
    void createWorld(std::string filename);
    /* Load the given map and start playing. */
    void play(std::string mapname);
public:
    /* Constructor. Takes the path to the folder containing the executable. */
    Game(std::string p);

    /* Do everything. */
    void run();
};

#endif
