#include "Game.hh"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    /* The name of the executable, without any path. */
    std::string executable = "burrowbun";
    std::string called = std::string(argv[0]);

    /* The path to the executable. */
    std::string path = called.substr(0, called.size() - executable.size());

    Game game(path);
    game.run();
    return 0;
}


