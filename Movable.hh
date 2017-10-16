#ifndef MOVABLE_HH
#define MOVABLE_HH

#include <string>
#include <set>
#include "Sprite.hh"
#include "json.hpp"
#include "Damage.hh"
#include "Rect.hh"

namespace movable {

// For holding an x and a y coordinate, but doubles instead of ints
struct Point {
    double x;
    double y;
};

/* Get a point out of a json. */
void from_json(const nlohmann::json &j, Point &point);

/*  A base class for anything that can move and collide on the map, 
    not including tiles. Monsters, NPCs, the player, and dropped items should
    all be subclasses. */
class Movable {
protected:
    /* Collision rectangle. */
    Rect rect;

public:
    /* Allow from_json access to private variables, since it is basically a
    factory function. */
    friend void from_json(const nlohmann::json &j, Movable &movable);

    // Movement-related fields
    // Drag is a number between 0 and 1 which the velocity is multiplied by
    Point drag, velocity, accel, dAccel;

    // Information about recent collisions
    // We need to know whether there was a collision down so that we can
    // avoid jumping midair, and we need isCollidingX to not be stopped by
    // one block high walls.
    bool isCollidingX;
    bool isCollidingDown;
    int ticksCollidingDown; // Number of updates isCollidingDown has been true
    bool isSteppingUp; // As in, stepping up one block
    int timeOffGround; // How many updates since it was on the ground
    bool collidePlatforms; // Whether or not to fall through platforms
    bool isDroppingDown; // isCollidingDown && !collidePlatforms
    // How many updates before holding down the jump key stops working. This
    // determines maximum jump height. Here -1 means infinity.
    int maxJumpTime; 

    // What number to use when calculating fall damage
    int pixelsFallen;
    // The highest point reached since the last jump, or since last touching 
    // the ground
    int maxHeight;

    /* The minimum downward velocity required to prevent the movable from 
    resetting its maxHeight. (To avoid taking fall damage, slow down to this
    speed.) */
    double minVelocity;

    // Location
    int x, y;

    /* How fast are all the boulders trying to move it this update. */
    int boulderSpeed;

    // Constructor
    Movable();

    /* Constructor from json. */
    Movable(std::string filename);

    /* Copy constructor. */
    Movable(const Movable &movable);

    /* Operator= */
    Movable &operator=(const Movable &movable);

    // Destructor
    virtual ~Movable();

    // Access functions
    Point getVelocity() const;
    void setVelocity(Point newVelocity); // Very few things should use this.
    void setAccel(Point newAccel);
    Point getDAccel() const;

    // Updates velocity
    void accelerate(double gravity);

    /* Take damage. Since movables in general don't have health, this mostly
    exists so the collider can tell movables to take damage from overlapping a
    tile without hacing to handle entities differently. */
    virtual void takeDamage(const Damage &damage);

    /* Take fall damage. Also does nothing unless the movables is an entity. */
    virtual void takeFallDamage();

    /* Convert a rectangle from world coordinates to screen coordinates. */
    static void convertRect(SDL_Rect &rect, const Rect &camera);

    /* Render itself to the screen, given a Rect that tells it where the
    screen is in the world. Since Movables don't have sprites, this is just
    here to be virtual. */
    virtual void render(const Rect &camera);

    /* Get height and width, defined by height and width of the sprite. */
    virtual int getWidth() const;
    virtual int getHeight() const;
    virtual Rect getRect() const;
};

/* Get a movable from a json file. */
void from_json(const nlohmann::json &j, Movable &movable);

} // End namespace movable

#endif
