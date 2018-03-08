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

    int screenWidth;
    int screenHeight;

    static std::vector<Button> getButtons(Screen s);

    void setButtons();

public:
    Menu();

    void update(int screenWidth, int screenHeight);

    void render();
};







































#endif
