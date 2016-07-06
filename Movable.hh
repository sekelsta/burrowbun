#ifndef MOVABLE_HH
#define MOVABLE_HH

#include <string>

using namespace std;

/* A base class for anything that can move and collide on the map, 
   not including tiles. */
class Movable {
protected:
    // Fields
    string sprite;

    // Movement-related fields
    double xMaxSpeed, yMaxSpeed, xVelocity, yVelocity;

public:
    // Location
    int x, y;

    // Acceleration
    double xAccel;
    double yAccel;

    // Access functions
    double getXVelocity();
    double getYVelocity();

    // Updates velocity
    void accelerate();
};

#endif
