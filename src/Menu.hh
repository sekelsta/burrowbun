#ifndef MENU_HH
#define MENU_HH

#include "Button.hh"
#include <vector>

enum class Screen {
    START
};

class Menu {
    Screen screen;
    std::vector<Button> buttons;

    static std::vector<Button> getButtons(Screen s);

public:
    Menu();

    void update();

    void render();
};







































#endif
