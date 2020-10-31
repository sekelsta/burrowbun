#ifndef GAME_HH
#define GAME_HH

#include <string>
#include "MapHelpers.hh"

class Menu;

class Game { 
    /* For capping the frame rate. */
    const uint32_t SCREEN_FPS;
    const uint32_t TICKS_PER_FRAME;

    /* The path to the folder containing the executable. */
    static std::string path;
public:
    /* Constructor. Takes the path to the folder containing the executable. */
    Game(std::string p);

    /* Get the path. */
    static inline std::string getPath() {
        return path;
    }

    void create_new_world(std::string filename);
};

#endif
