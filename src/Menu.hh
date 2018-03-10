#ifndef MENU_HH
#define MENU_HH

#include "Button.hh"
#include "EventHandler.hh"
#include "MapHelpers.hh"
#include "Mapgen.hh"
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

    void dofun(Menu &menu) {
        if (containsMouse && wasClicked && !isHeld) {
            fun(menu);
            reset();
        }
    }
};

class Menu {
    friend void EventHandler::updateMenu(Menu &menu);

    Screen state;
    std::vector<Buttonfun> buttons;


    int screenWidth;
    int screenHeight;

    /* Create a new world. */
    void createWorld(std::string filename, WorldType type);

    /* Get the list of buttons the given state should have. */
    static std::vector<Buttonfun> getButtons(Screen s);

    /* Position the buttons based on screen size. */
    void setButtons();

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
