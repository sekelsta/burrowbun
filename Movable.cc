#include <cassert>
#include "Movable.hh"
#include <iostream>

using namespace std;

// Constructor
Movable::Movable() {
    xVelocity = 0;
    yVelocity = 0;
    xAccel = 0;
    yAccel = 0;

    // These should be changed by the child class's init.
    xMaxSpeed = 0;
    yMaxSpeed = 0;
}

double Movable::getXVelocity() {
    return xVelocity;
}

double Movable::getYVelocity() {
    return yVelocity;
}

// This adds acceleration to speed, and limits speed at maxSpeed.
void Movable::accelerate() {
    // Update velocity
    xVelocity += xAccel;
    yVelocity += yAccel;

    // Make sure velocity is less than max velocity
    xVelocity = min(xVelocity, xMaxSpeed);
    yVelocity = min(yVelocity, yMaxSpeed);
    xVelocity = max(xVelocity, -1 * xMaxSpeed);
    yVelocity = max(yVelocity, -1 * yMaxSpeed);
}

