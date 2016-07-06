#include <cassert>
#include "Movable.hh"

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
    xVelocity = min(xVelocity, xMaxSpeed);
    yVelocity = min(yVelocity, yMaxSpeed);
    xVelocity = max(xVelocity, -1 * xMaxSpeed);
    yVelocity = max(yVelocity, -1 * yMaxSpeed);
}

