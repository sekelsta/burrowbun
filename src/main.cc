#include "Game.hh"
#include <iostream>
#include <string>
#include <libgen.h> // For dirname
#include <unistd.h> // For readlink
#include "Mapgen.hh"


using namespace std;

int main(int argc, char **argv) {
    /* The path to the executable, linux-only. */
    int PATH_MAX = 512;
    char result[ PATH_MAX ];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX - 1);
    std::string path;
    if (count != -1) {
        result[count] = '\0';
        path = dirname(result);
    }
    else {
        cerr << "Unable to get path to executable!\nExiting...\n";
        exit(1);
    }
    path = path + "/";
    Game game(path);

    if (argc == 3 && strcmp(argv[1], "-c") == 0) {
        game.create_new_world(argv[2]);
    }
    else {
        cout << "Usage:\n    " << argv[0] << "\n    to run the game (disabled due to lack of SDL), or\n    "
        << argv[0] << " -c worldname\n    to create a world with the given file name.\n";
    }
    return 0;
}


