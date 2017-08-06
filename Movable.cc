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
    texture = NULL;
    isCollidingDown = false;
    ticksCollidingDown = 0;
    isCollidingX = false;
    isSteppingUp = false;
    timeOffGround = 0;
    collidePlatforms = true;
    isDroppingDown = false;

    pixelsFallen = 0;
    maxHeight = 0;

    gravity = 0;

    // These should be changed by the child class's init.
    drag.x = 0;
    drag.y = 0;
    // The amount to accelerate by when trying to move
    dAccel.x = 0;
    dAccel.y = 0;
    sprite = "";
    spriteWidth = 0;
    spriteHeight = 0;
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

string Movable::getSprite() const {
    return sprite;
}

// This adds acceleration to speed, and limits speed at maxSpeed. This also
// updates the value of timeOffGround.
void Movable::accelerate() {
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
void Movable::takeDamage(int normal, int wounds) {}

/* Take fall damage. Does nothing. */
void Movable::takeFallDamage() {}
