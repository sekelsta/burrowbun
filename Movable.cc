#include <cassert>
#include <cmath> // For rounding
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

    // Don't bother going ridiculously slowly
    if (-1 < velocity.x && velocity.x < 1) {
        velocity.x = 0;
    }
    if (-1 < velocity.y && velocity.y < 1) {
        velocity.y = 0;
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

