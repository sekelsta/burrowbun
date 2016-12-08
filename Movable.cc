#include <cassert>
#include "Movable.hh"
#include <iostream>

using namespace std;

// Constructor
Movable::Movable() {
    velocity.x = 0;
    velocity.y = 0;
    accel.x = 0;
    accel.y = 0;

    // These should be changed by the child class's init.
    drag.x = 0;
    drag.y = 0;
    dAccel.x = 0;
    dAccel.y = 0;
}

Point Movable::getVelocity() {
    return velocity;
}

void Movable::setAccel(Point newAccel) {
    accel = newAccel;
}

Point Movable::getDAccel() {
    return dAccel;
}

// This adds acceleration to speed, and limits speed at maxSpeed.
void Movable::accelerate() {
    // Update velocity
    velocity.x += accel.x;
    velocity.y += accel.y;

    // Add drag effects
    velocity.x *= drag.x;
    velocity.y *= drag.y;
}

