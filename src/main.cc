#include "Game.hh"
#include "render/Texture.hh"

using namespace std;

int main(int argc, char **argv) {
    Game game;
    game.run();
    // Clean up fonts
    Texture::closeFonts();
    return 0;
}


