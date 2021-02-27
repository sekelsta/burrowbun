#ifndef MENU_HH
#define MENU_HH

#include "Button.hh"
#include "../EventHandler.hh"
#include "../world/MapHelpers.hh"
#include "../world/Mapgen.hh"
#include <vector>
#include <string>
#include <functional>
#include <thread>
#include <mutex>

enum class Screen {
    START,
    PLAY,
    CREATE,
    QUIT
};

/* A struct to hold information about what to do when clicked on. */
struct Buttonfun: public Button {
    std::function<void(Menu &)> fun;

    inline void dofun(Menu &menu) {
        if (containsMouse && wasClicked && !isHeld) {
            fun(menu);
            reset();
        }
    }
};

/* Like a sprite, but it has a rect for where it is on the screen. */
struct Spriterect {
    Sprite sprite;
    Rect rect;

    inline void render() {
        sprite.render(rect);
    }
};

/* A class to do menu stuff. */
class Menu {
    friend void EventHandler::updateMenu(Menu &menu);

    Screen state;
    std::vector<Buttonfun> buttons;
    std::vector<Spriterect> sprites;


    int screenWidth;
    int screenHeight;

    /* Create a new world. */
    void createWorld(std::string filename, WorldType type);

    /* Get the list of buttons the given state should have. */
    static std::vector<Buttonfun> getButtons(Screen s);

    /* Get the list of sprites the given state should have. */
    std::vector<Spriterect> getSprites();

    /* Position the buttons based on screen size. */
    void setButtons();

    /* Position the sprites based on screen size. */
    void setSprites();

    /* How far along world creation is. */
    CreateState create;

    /* Thread for creating a world */
    std::thread *t;
    std::mutex m;

public:
    Menu();

    /* Access function. */
    inline Screen getState() {
        return state;
    }

    void setState(Screen newstate);

    inline std::string getFilename() {
        // TODO
        return "world.world";
    }

    /* Update state. */
    void update(int screenWidth, int screenHeight);

    /* Also self-explanatory. */
    void render();
};







































#endif
