#include "Game.hh"
#include <iostream>

int main(int argc, char **argv) {
    std::cout << argv[0] << "\n";
    Game game;
    game.run();
    return 0;
}


