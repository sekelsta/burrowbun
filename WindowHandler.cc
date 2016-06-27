#include "WindowHandler.hh"

using namespace std;

// Return a rectangle in world coordinates with x and y at the center
SDL_Rect WindowHandler::findCamera(int x, int y) {
    SDL_Rect camera;
    camera.x = x - SCREEN_WIDTH / 2;
    camera.y = y - SCREEN_HEIGHT / 2;
    camera.w = SCREEN_WIDTH;
    camera.h = SCREEN_HEIGHT;

    // Adjust the camera if necessary, so that it's entirely on the screen
    camera.x = min(camera.x, worldWidth - SCREEN_WIDTH);
    camera.y = min(camera.y, worldHeight - SCREEN_HEIGHT);
    camera.x = max(0, camera.x);
    camera.y = max(0, camera.y);

    // If, God forbid, the map is smaller than the camera, shrink the camera
    camera.w = min(camera.w, worldWidth - TILE_HEIGHT);
    camera.h = min(camera.h, worldHeight - TILE_WIDTH);

    return camera;
}

// Convert a rectangle from world coordinates to screen coordinates
SDL_Rect WindowHandler::convertRect(SDL_Rect rect, int x, int y) {
    SDL_Rect camera = findCamera(x, y);
    int xScreen = rect.x - camera.x;
    int yScreen = camera.y + SCREEN_HEIGHT - rect.y;
    SDL_Rect screenRect = { xScreen, yScreen, rect.w, rect.h };

    return screenRect;
}

// Constructor
WindowHandler::WindowHandler(int screenWidth, int screenHeight, 
                                int mapWidth, int mapHeight) 
    : SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight), 
        TILE_WIDTH(20), TILE_HEIGHT(20), TILE_PATH("content/"), 
        worldWidth(TILE_WIDTH * mapWidth),
        worldHeight(TILE_HEIGHT * mapHeight) {
    window = NULL;
    screenSurface = NULL;
    renderer = NULL;

    // Set the 2D vector of rects for the tiles
    // Access goes [x][y]
    tileRects.resize((SCREEN_WIDTH / TILE_WIDTH) + 1);
    for (unsigned i = 0; i < tileRects.size(); i++) {
        tileRects[i].resize((SCREEN_HEIGHT / TILE_HEIGHT) + 1);
    }
}

// Start up the window
bool WindowHandler::init() {
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize. SDL_Error: ";
        cerr << SDL_GetError() << endl;
        success = false;
    }
    else {
        // Create window
        window = SDL_CreateWindow("Hello whirreled!", SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                    SDL_WINDOW_SHOWN);
        if (window == NULL) {
            cerr << "Window could not be created. SDL_Error: ";
            cerr << SDL_GetError() << endl;
            success = false;
        }
        else {
            // Create a renderer for the window
            renderer = SDL_CreateRenderer(window, -1, 
                                            SDL_RENDERER_ACCELERATED);

            // Fall back to a software renderer if necessary
            if (renderer == NULL) {
                cerr << "Hardware-accelerated renderer could not be created. ";
                cerr << "SDL Error: " << endl << SDL_GetError() << endl;
                cerr << "Falling back to software renderer." << endl;
                renderer = SDL_CreateRenderer(window, -1, 
                                            SDL_RENDERER_SOFTWARE);
            }
            if (renderer == NULL) {
                cerr << "Software-accelerated renderer could not be created. ";
                cerr << "SDL Error: " << endl << SDL_GetError() << endl;
                success = false;
            }
            else {
                // Initialize renderer draw color
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    cerr << "SDL_image could not initialize. SDL_image Error: ";
                    cerr << endl << IMG_GetError() << endl;
                    success = false;
                }
            }
        }
    }
    return success;
}

// Load test.bmp to surface, return true if successful
bool WindowHandler::loadMedia(vector<Tile *> &pointers) {
    bool success = true;

    // Load the textures for tiles
    loadTiles(pointers);

    return success;
}

// Load an image onto a surface and convert it to match the screen
SDL_Texture *WindowHandler::loadTexture(const string &name) {
    // Declare variables and load a surface
    SDL_Surface *surface = IMG_Load(name.c_str());
    SDL_Texture *texture = NULL;
    if (surface == NULL) {
        cerr << "Unable to load image " << name << endl;
        cerr << "SDLError: " << SDL_GetError() << endl;
    }
    // Make a texture
    else {
        // Convert the surface to a texture
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == NULL) {
            cerr << "Failed to crate texture from " <<  name << endl;
            cerr << "SDL Error: " << SDL_GetError() << endl;
        }

        // Get rid of the surface
        SDL_FreeSurface(surface);
    }

    // Add it to the list of textures so it can be deallocated later
    textures.push_back(texture);

    return texture;
}

// Load a texture for each tile
bool WindowHandler::loadTiles(vector<Tile *> &pointers) {
    bool success = true;
    for (unsigned i = 0; i < pointers.size(); i++) {
        string name = TILE_PATH + pointers[i] -> sprite;
        loadTexture(name);
        assert(textures.size() != 0);
        if (textures.back() == NULL) {
            success = false;
        }

        // Set the tile's texture to the loaded texture
        assert(pointers[i] -> texture == NULL);
        pointers[i] -> texture = textures.back();
    }

    return success;
}

// Render everything the map holds information about
// x and y are the center of view of the camera, in pixels, 
// where y = 0 at the bottom
// If either value puts the camera past the end of the map, it will be fixed
void WindowHandler::renderMap(const Map &m, unsigned x, unsigned y) {
    // Make sure the renerer draw color is set to white
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Find the camera
    SDL_Rect camera = findCamera(x, y);

    // Rectangle to draw to
    SDL_Rect *rectTo;

    // Iterate through every tile at least partially within the camera
    for (int i = 0; i < (camera.w / TILE_WIDTH) + 1; i++) {
        int xRectTo = i * TILE_WIDTH - (camera.x % TILE_WIDTH);
        for (int j = 0; j < (camera.h / TILE_HEIGHT) + 1; j++) {
            // Set the rectangle to the correct one in the vector2D
            rectTo = &tileRects[i][j];

            rectTo -> w = TILE_WIDTH;
            rectTo -> x = xRectTo;

            // Remember that screen y == 0 at the top but world y == 0 at 
            // the bottom. Here j == 0 at the top of the screen.
            // We're not using convertRect because that doesn't align them
            // wit hthe tile grid.
            rectTo -> h = TILE_HEIGHT;
            rectTo -> y = (camera.h + camera.y) % TILE_HEIGHT;
            rectTo -> y += (j - 1) * TILE_HEIGHT;

            // Render the tile
            int xTile = (camera.x / TILE_WIDTH) + i;
            int yTile = (camera.y + camera.h) / TILE_HEIGHT - j;
            // But only if it's a tile that exists on the map
            if (0 <= xTile && xTile < worldWidth / TILE_WIDTH 
                && 0 <= yTile && yTile < worldHeight / TILE_HEIGHT) { 
                Tile *tile = m.getTile(xTile, yTile);
                SDL_RenderCopy(renderer, tile -> texture, NULL, rectTo);
            }
            else {
                cerr << "Tried to render the tile at " << xTile;
                cerr << ", " << yTile << endl;
                cerr << "camera.x is " << camera.x << ", camera.y is ";
                cerr << camera.y << ", camera.w is " << camera.w;
                cerr << ", camera.h is " << camera.h << endl;
            }
        }
    }
}

// Update the screen
void WindowHandler::update(const Map &m) {
    // Clear the screen
    SDL_RenderClear(renderer);

    // Put a black rectangle in the background
    SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);

    // Put stuff on it
    renderMap(m, m.getSpawn().x * TILE_WIDTH, m.getSpawn().y * TILE_HEIGHT);
    /*
    SDL_Rect stretch = { 0, 0, 20, 20 };
    SDL_RenderCopy(renderer, textures.back(), NULL, NULL);
    SDL_RenderCopy(renderer, textures.back(), NULL, &stretch);
    */

    // Update the screen
    SDL_RenderPresent(renderer);
}

// Close the window, clean up, and exit SDL
void WindowHandler::close() {
    // Deallocate any textures
    for(unsigned i = 0; i < textures.size(); i++) {
        // If I were being a good programmer I'd make the pointers
        // point to NULL
        SDL_DestroyTexture(textures[i]);
    }

    // Destroy window
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}
