#include "Game.hh"

#include <iostream>
#include <cassert>
#include "Tile.hh"
#include "Mapgen.hh"
#include "tile_size.hh"

using namespace std;

string Game::path;


Game::Game(string p) : SCREEN_FPS(60), TICKS_PER_FRAME(1000 / SCREEN_FPS) {
    path = p;
}


void Game::create_new_world(std::string filename) {
    Mapgen mapgen(path);
    mapgen.generate(filename + ".world", WorldType::EARTH, path, nullptr, nullptr);
}

