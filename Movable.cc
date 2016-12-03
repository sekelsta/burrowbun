#include <cassert>
#include "Movable.hh"
#include <iostream>

using namespace std;

double Movable::getXVelocity() {
    return xVelocity;
}

double Movable::getYVelocity() {
    return yVelocity;
}

void Movable::accelerate() {
    // Update velocity
    xVelocity += xAccel;
    yVelocity += yAccel;

    // Make sure velocity is less than max velocity
    // TODO: xVelocity is uninitialized. I need to learn more about 
    // inheritance.
    xVelocity = min(xVelocity, xMaxSpeed);
    yVelocity = min(yVelocity, yMaxSpeed);
    xVelocity = max(xVelocity, -1 * xMaxSpeed);
    yVelocity = max(yVelocity, -1 * yMaxSpeed);
}

