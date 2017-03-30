#ifndef SPRITE_HH
#define SPRITE_HH

#include <string>

// Forward declare
struct SDL_Texture;

struct Sprite {
    // The name of the sprite image. This may be a spritesheet.
    std::string name;
    SDL_Texture *texture;

    // The width and height of the sprite
    int width;
    int height;

    // If this is a spritesheet, the number of rows and columns
    int rows;
    int cols;

    // When referring to specific square, it's row and column (0 indexed)
    int row;
    int col;
};

#endif
