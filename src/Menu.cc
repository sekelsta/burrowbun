#include "Menu.hh"

using namespace std;

vector<Button> Menu::getButtons(Screen s) {
    vector<Button> b;
    if (s == Screen::START) {
        // TODO
    }
    return b;
}

Menu::Menu() {
    screen = Screen::START;

    buttons = getButtons(screen);
}

void Menu::render() {
    for (unsigned int i = 0; i < buttons.size(); i++) {
        buttons[i].render();
    }
}


