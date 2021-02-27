#ifndef ITEM_HH
#define ITEM_HH

#include <string>
#include "Action.hh"



/* The thing inventories store. */
class Item : public Action {
protected:
    /* How many can be in a stack in the same slot. */
    int maxStack;

    /* How many are in this stack. */
    int stack;

    /* Whether it gets used up when used. */
    bool consumable;

    /* Virtual use function. Does nothing, returns false. */
    virtual bool use_internal(InputType type, int x, int y, World &world);

public:
    // Constructor
    Item(ActionType type, std::string path);

    /* Use function. Decreases number if consumable, and calls use_internal(). */
    void use(InputType type, int x, int y, World &world);

    /* Destructor must be virtual. */
    virtual ~Item();

    /* Access functions. */
    inline int getStack() {
        return stack;
    }

    inline void setStack(int s) {
        stack = s;
    }

    inline int isConsumable() {
        return consumable;
    }

    /* Render itself. */
    virtual void render(SDL_Rect &rect, std::string path);

    /* Move n of the stack from other to this. n can be negative. 
    If n is 0, merge as many as possible. Return the item for the other stack,
    or nullptr if it was merged completely. */
    Item *merge(Item *other, int n);

    /* Get json filename from ActionType. */
    static std::string getJsonFilename(ActionType type);
};

#endif
