#include "Menu.hh"
#include <iostream>

#define MENU_BUTTON_SIZE 48
#define MENU_TEXT_SIZE 32

using namespace std;


void Menu::createWorld(string filename, WorldType type) {
    string path = Texture::getPath();
    Mapgen mapgen(path);
    mapgen.generate(path + filename, type, path, &create, &m);
}

vector<Buttonfun> Menu::getButtons(Screen s) {
    vector<Buttonfun> b;
    if (s == Screen::START) {
        b.resize(3);
        b[0].sprite = Sprite(Texture("Play", MENU_BUTTON_SIZE, 0));
        b[0].fun = [](Menu &menu) {
            menu.setState(Screen::PLAY);
        };
        b[1].sprite = Sprite(Texture("Create World", MENU_BUTTON_SIZE, 0));
        b[1].fun = [](Menu &menu) {
            menu.setState(Screen::CREATE);
        };
        b[2].sprite = Sprite(Texture("Quit", MENU_BUTTON_SIZE, 0));
        b[2].fun = [](Menu &menu) {
            menu.setState(Screen::QUIT);
        };
    }
    return b;
}

std::vector<Spriterect> Menu::getSprites() {
    vector<Spriterect> sprites;
    if (state == Screen::PLAY) {
        sprites.resize(1);
        sprites[0].sprite = Sprite(Texture("Loading map...", 
            MENU_BUTTON_SIZE, 0));
    }
    else if (state == Screen::CREATE) {
        sprites.resize(2);
        sprites[0].sprite = Sprite(Texture("Creating new world...", 
            MENU_BUTTON_SIZE, 0));
        string message = "";
        m.lock();
        switch (create) {
            case CreateState::NOT_STARTED:
                message = "Starting...";
                break;
            case CreateState::STUFF:
                message = "Doing stuff...";
            case CreateState::GENERATING_BIOMES:
                message = "Setting biomes...";
                break;
            case CreateState::GENERATING_TERRAIN:
                message = "Placing blocks...";
                break;
            case CreateState::SETTLING_WATER:
                message = "Settling water...";
                break;
            case CreateState::SAVING:
                message = "Saving generated map...";
                break;
            case CreateState::DONE:
                message = "Finished!";
                break;
            case CreateState::NONE:
                message = "Error?";
                break;
        }
        m.unlock();
        sprites[1].sprite = Sprite(Texture(message, MENU_TEXT_SIZE, 0));
    }
    return sprites;
}

void Menu::setButtons() {
    Light mouse = {0xFF, 0xCC, 0x00, 0xFF};
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

void Menu::setSprites() {
    int space = 16;
    int total = 0;
    for (unsigned int i = 0; i < sprites.size(); i++) {
        total += sprites[i].sprite.getHeight();
    }

    int y = (screenHeight - total - (space * sprites.size())) / 2;
    for (unsigned int i = 0; i < sprites.size(); i++) {
        sprites[i].rect.w = sprites[i].sprite.getWidth();
        sprites[i].rect.h = sprites[i].sprite.getHeight();
        sprites[i].rect.x = (screenWidth - sprites[i].rect.w) / 2;
        sprites[i].rect.y = y;
        y += sprites[i].rect.h + space;
    }
}

Menu::Menu() {
    screenWidth = 0;
    screenHeight = 0;
    setState(Screen::START);
    create = CreateState::NONE;
    t = nullptr;
}

void Menu::setState(Screen newstate) {
    state = newstate;
    buttons = getButtons(state);
    sprites = getSprites();
    setButtons();
    setSprites();
}

void Menu::update(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    setButtons();
    setSprites();

    for (unsigned int i = 0; i < buttons.size(); i++) {
        buttons[i].dofun(*this);
    }

    if (state == Screen::CREATE) {
        m.lock();
        if (create == CreateState::NONE) {
            create = CreateState::NOT_STARTED;
            assert(t == nullptr);
            t = new thread(&Menu::createWorld, this, getFilename(), 
                WorldType::EARTH);
        }
        else if (create == CreateState::DONE) {
            assert(t);
            t -> join();
            delete t;
            t = nullptr;
            setState(Screen::START);
            create = CreateState::NONE;
        }
        m.unlock();
    }

    m.lock();
    if (create != CreateState::NONE) {
        m.unlock();
        sprites = getSprites();
        m.lock();
        setSprites();
    }
    m.unlock();
}

void Menu::render() {
    // Make sure the renderer isn't rendering to a texture
    Renderer::setTarget(NULL);

    // Put a sky-colored rectangle in the background
    // TODO: remove magic numbers
    Renderer::setColor(0x00, 0x99, 0xFF, 0xFF);
    Renderer::renderClear();

    for (unsigned int i = 0; i < buttons.size(); i++) {
        buttons[i].render();
    }

    for (unsigned int i = 0; i < sprites.size(); i++) {
        sprites[i].render();
    }

    // Update the screen
    Renderer::renderPresent();
}


