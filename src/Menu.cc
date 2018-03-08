#include "Menu.hh"

#define MENU_BUTTON_SIZE 32

using namespace std;

vector<Button> Menu::getButtons(Screen s) {
    vector<Button> b;
    if (s == Screen::START) {
        b.resize(3);
        b[0].sprite = Sprite(Texture("Play", MENU_BUTTON_SIZE, 0));
        b[1].sprite = Sprite(Texture("Create World", MENU_BUTTON_SIZE, 0));
        b[2].sprite = Sprite(Texture("Quit", MENU_BUTTON_SIZE, 0));
    }
    return b;
}

void Menu::setButtons() {
    Light mouse = {0xCC, 0xFF, 0x00, 0xFF};
    Light noMouse = {0xFF, 0xFF, 0xFF, 0xFF};
    int space = 16;
    int start = 64;
    

    int y = start;
    for (unsigned int i = 0; i < buttons.size(); i++) {
        buttons[i].mouse = mouse;
        buttons[i].noMouse = noMouse;
        int w = buttons[i].sprite.getWidth();
        int h = buttons[i].sprite.getHeight();
        int x = (screenWidth - w) / 2;
        buttons[i].move(x, y, w, h);
        y += h + space;
    }
}

Menu::Menu() {
    screenWidth = 0;
    screenHeight = 0;
    screen = Screen::START;

    buttons = getButtons(screen);
}

void Menu::update(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    setButtons();
}

void Menu::render() {
    // Make sure the renderer isn't rendering to a texture
    SDL_SetRenderTarget(Renderer::renderer, NULL);
    // Clear the screen
    SDL_RenderClear(Renderer::renderer);
    // Put a sky-colored rectangle in the background
    SDL_Rect fillRect = { 0, 0, screenWidth, screenHeight };
    // TODO: remove magic numbers
    SDL_SetRenderDrawColor(Renderer::renderer, 0x00, 0x99, 0xFF, 0xFF);
    SDL_RenderFillRect(Renderer::renderer, &fillRect);

    for (unsigned int i = 0; i < buttons.size(); i++) {
        buttons[i].render();
    }

    // Update the screen
    SDL_RenderPresent(Renderer::renderer);
}


