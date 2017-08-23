#include <cassert>
#include <cmath> // For rounding
#include <fstream>
#include "Movable.hh"
#include <iostream>

using namespace std;
using json = nlohmann::json;

namespace movable {

/* Get a point from a json file. */
void from_json(const json &j, Point &point) {
    point.x = j["x"];
    point.y = j["y"];
}

// Constructor
Movable::Movable() {
    velocity.x = 0;
    velocity.y = 0;
    accel.x = 0;
    accel.y = 0;
    isCollidingDown = false;
    ticksCollidingDown = 0;
    isCollidingX = false;
    isSteppingUp = false;
    timeOffGround = 0;
    collidePlatforms = true;
    isDroppingDown = false;

    pixelsFallen = 0;
    maxHeight = 0;
    boulderSpeed = 0;

    // These should be changed by the child class's init.
    drag.x = 0;
    drag.y = 0;
    // The amount to accelerate by when trying to move
    dAccel.x = 0;
    dAccel.y = 0;
}

/* Constructor from json file. */
Movable::Movable(std::string filename) {
    /* open file */
    ifstream infile(filename);
    /* Put data in json. */
    json j = json::parse(infile);
    *this = j.get<Movable>();
    assert(sprite.name != "");
}

// Virtual destructor
Movable::~Movable() {};

// Access functions

Point Movable::getVelocity() const {
    return velocity;
}

void Movable::setVelocity(Point newVelocity) {
    velocity = newVelocity;
}

void Movable::setAccel(Point newAccel) {
    accel = newAccel;
}

Point Movable::getDAccel() const {
    return dAccel;
}

// This adds acceleration to speed, and limits speed at maxSpeed. This also
// updates the value of timeOffGround and maxHeight.
void Movable::accelerate(double gravity) {
    // If on the ground, timeOffGround should be 0, otherwise it should be
    // one more than it was before
    if (isCollidingDown) {
        timeOffGround = 0;
    }
    else {
        timeOffGround++;
    }

    // Update velocity
    velocity.x += accel.x;
    velocity.y += accel.y;
    if (!isSteppingUp) {
        velocity.y += gravity;
    }

    // Add drag effects
    velocity.x *= drag.x;
    velocity.y *= drag.y;

    // Don't bother going ridiculously slowly
    if (-1 < velocity.x && velocity.x < 1) {
        velocity.x = 0;
    }
    if (-1 < velocity.y && velocity.y < 1) {
        velocity.y = 0;
    }
    
    /* Reset maxHeight if it isn't falling fast enough. */
    if (velocity.y > minVelocity) {
        maxHeight = min(maxHeight, y);
    }

    // Since location is an int, make velocity an int by rounding away from 0
    if (velocity.x < 0) {
        velocity.x = floor(velocity.x);
    }
    velocity.x = ceil(velocity.x);

    if (velocity.y < 0) {
        velocity.y = floor(velocity.y);
    }
    velocity.y = ceil(velocity.y);
}

/* Take damage. Does nothing. */
void Movable::takeDamage(const Damage &damage) {}

/* Take fall damage. Does nothing. */
void Movable::takeFallDamage() {}

/* Get a movable from a json file. */
void from_json(const json &j, Movable &movable) {
    movable.sprite = j["sprite"].get<Sprite>();
    movable.drag = j["drag"].get<Point>();
    assert(movable.drag.x <= 1.0);
    assert(movable.drag.y <= 1.0);
    assert(0 < movable.drag.x);
    assert(0 < movable.drag.y);
    movable.velocity = j["velocity"].get<Point>();
    movable.accel.x = 0;
    movable.accel.y = 0;
    movable.dAccel = j["dAccel"].get<Point>();
    movable.isCollidingX = false;
    movable.isCollidingDown = false;
    movable.ticksCollidingDown = 0;
    movable.isSteppingUp = false;
    movable.timeOffGround = j["timeOffGround"];
    movable.collidePlatforms = true;
    movable.isDroppingDown = false;
    movable.maxJumpTime = j["maxJumpTime"];
    movable.pixelsFallen = j["pixelsFallen"];
    movable.maxHeight = j["maxHeight"];
    movable.minVelocity = j["minVelocity"];
    movable.x = j["x"];
    movable.y = j["y"];
    movable.boulderSpeed = 0;
}

} // End namespace movable
