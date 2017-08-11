#include <cassert>
#include "Movable.hh"

/* Rectangle, capable of seeing if another intersects it taking into account
    the world wrapping around the x direction. */
class Rect {
public:
    int worldWidth;
    int x;
    int y;
    int w;
    int h;

    /* Returns true if only adjustments in the y direction are needed to
    make the rectangles intersect. */
    inline bool intersectsX(const Rect &that) const {
        /* Get x values within the correct range. */
        int thisX = (x + worldWidth) % worldWidth;
        int thatX = (that.x + worldWidth) % worldWidth;
        assert(0 <= thisX);
        assert(0 <= w);
        assert(0 <= thatX);
        assert(0 <= that.w);
        assert(thisX < worldWidth);
        assert(thatX < worldWidth);
        assert(worldWidth != 0);
        assert(that.worldWidth != 0);

        bool intersectsX = (thisX + w > thatX) && (thisX < thatX + that.w);

        assert(intersectsX == (thisX >= thatX && thisX < thatX + that.w) 
                || (thatX < thisX + w && thatX >= thisX));
        /* If either rectangle wraps but not both, we should also see if they
        intersect when one is moved to the other side of the line. 
        (The != is an xor. ) */
        if ((thisX + w < worldWidth) != (thatX + that.w < worldWidth)) {
            bool thisWraps = (thatX + worldWidth < thisX + w)
                    && (thisX < thatX + that.w + worldWidth);
            bool thatWraps = (thatX < thisX + w + worldWidth) 
                    && (thisX + worldWidth < thatX + that.w);
            intersectsX = intersectsX || thisWraps || thatWraps;
        }
        
        return intersectsX; 
    }

    /* Returns true if only adjustments in the x direction are needed to 
    make the rectangles intersect. */
    inline bool intersectsY(const Rect &that) const {
        assert(0 <= y);
        assert(0 <= that.y);
        assert(0 <= h);
        assert(0 <= that.h);

        return (y + h > that.y) && (y < that.y + that.h);
    }

    /* Function to tell whether another rectangle intersects this one.
    Note that it may be the case that one rectangle or other has x < 0
    or x > worldWidth. */
    inline bool intersects(const Rect &that) const {
        return intersectsX(that) && intersectsY(that);
    }

    /* Collide with the top or bottom of the map. */
    inline void collideEdge(movable::Movable &movable, int worldHeight) {
        if (y <= 0) {
            movable.isCollidingDown = true;
            y = 0;
        }
        y = std::min(y, worldHeight - h - 1);
    }
};


