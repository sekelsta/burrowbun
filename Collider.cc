#include <iostream>
#include "Collider.hh"

using namespace std;

// Constructor
Collider::Collider(int tileWidth, int tileHeight) : TILE_WIDTH(tileWidth),
    TILE_HEIGHT(tileHeight) {
    // Or disable collisions to get a map viewer
    enableCollisions = true;
    xOffset = 1;
    yOffset = 1;
}

// Given that a collision happens left or right, update info accordingly.
inline void Collider::findXCollision(CollisionInfo &info, int dx, 
        int w, const Rect &stays) const {
    // Collision is left or right
    if (dx < 0) {
        info.type = CollisionType::LEFT;
        info.x = stays.x + stays.w;
    }
    else {
        info.type = CollisionType::RIGHT;
        info.x = stays.x - w;
    }

}

// Given that a collision is up or down, update info accordingly
inline void Collider::findYCollision(CollisionInfo &info, int dy, 
        int h, const Rect &stays) const {
    // Collision is up or down
    if (dy < 0) {
        info.type = CollisionType::DOWN;
        info.y = stays.y + stays.h;
    }
    else {
        info.type = CollisionType::UP;
        info.y = stays.y - h;
    }
}

// Return information about a collision between a moving thing and a non-moving
// thing. 
CollisionInfo Collider::findCollision(const Rect &to, const Rect &stays, 
        int dx, int dy) const {
    CollisionInfo info;
    info.x = 0;
    info.y = 0;
    info.xCoefficient = 1;
    info.yCoefficient = 1;
    info.isInevitable = false;

    // Check for collisions
    if (stays.intersects(to)) {
        // There's a collision.
        // If exactly one of the sides didn't use to be in the collision area
        // and now it is, we know the direction. (Actually, it can be two
        // parallel ones, since we know velocity.)
        // If the left or right side started off in the collision area
        if ((stays.x >= to.x - dx && stays.x < to.x + to.w - dx)
                || (to.x - dx < stays.x + stays.w && to.x - dx >= stays.x)
                || (to.x + to.w - dx < stays.x + stays.w 
                && to.x + to.w - dx > stays.x)) {
            // Since this should only be called for things that didn't start 
            // already colliding, we know neither y side started in the 
            // collision area.
            assert(to.y - dy >= stays.y + stays.h || to.y - dy < stays.y);
            assert(to.y + to.h - dy > stays.y + stays.h
                || to.y + to.h - dy <= stays.y);
            // Since there definately is a collision, it has to be up or down
            assert(dy != 0);
            findYCollision(info, dy, to.h, stays);
            // Whether a collision happens doesn't depend on x velocity
            info.isInevitable = true; 
            return info;
        }
        // If the top or bottom started off in the collision area
        else if ((stays.y >= to.y - dy && stays.y < to.y + to.h - dy)
                || (to.y - dy < stays.y + stays.h && to.y - dy >= stays.y) 
                || (to.y + to.h - dy < stays.y + stays.h
                && to.y + to.h - dy > stays.y)) {
            // Likewise, we know it must be left or right
            assert(dx != 0);
            findXCollision(info, dx, to.w, stays);
            // And whether it happens doesn't dedpend on y velocity
            info.isInevitable = true;
            return info;
        }
        // Now we know none of the edges started off in the collision area,
        // so since we know there is a collision, there must be at least two
        // edges in the collision area.
        // Also for any of these cases isInevitable should stay false.
        int tx = min(abs(to.x - dx - stays.x - stays.w), 
            abs(to.x - dx - to.w - stays.x));
        int ty = min(abs(to.y - dy - stays.y - stays.h),
            abs(to.y - dy - to.h - stays.y));
        // Multiplying is more efficient than dividing, and only their
        // relative values matter anyway
        tx *= abs(dy);
        ty *= abs(dx);
        if (tx < ty) {
            // Collision is left or right
            findXCollision(info, dx, to.w, stays);
        }
        else if (ty < tx) {
            // Collision is up or down
            findYCollision(info, dy, to.h, stays);
        }
        // tx == ty and it's exactly hitting the corner
        else {
            findXCollision(info, dx, to.w, stays);
            if (info.type == CollisionType::LEFT) {
                info.type = CollisionType::LEFT_CORNER;
            }
            else {
                info.type = CollisionType::RIGHT_CORNER;
            }
        }
    }
    else {
        info.type = CollisionType::NONE;
    }

    return info;
}

/* Goes through the tiles near the player and adds all collisions found to 
the vector collisions. If any are inevitable, it returns true. Otherwise, it
returns false. */
bool Collider::listCollisions(vector<CollisionInfo> &collisions, const Map &map,
    const Rect &to, const Rect &from) const {
    // The rectangle of the current tile to check
    Rect stays;
    stays.w = TILE_WIDTH - 2 * xOffset;
    stays.h = TILE_HEIGHT - 2 * yOffset;
    int worldWidth = map.getWidth() * TILE_WIDTH;
    stays.worldWidth = worldWidth;

    // Calculate the distance we move.
    int dx = to.x - from.x;
    int dy = to.y - from.y;

    bool anyInevitable = false;

    // Only these tiles can possibly be colliding
    for (int k = to.x / TILE_WIDTH;
            k < (to.x + to.w) / TILE_WIDTH + 1; k++) {
        int l = (k + map.getWidth()) % map.getWidth();
        stays.x = l * TILE_WIDTH + xOffset;
        for (int j = to.y / TILE_HEIGHT;
                j < (to.y + to.h) / TILE_HEIGHT + 1; j++) {
            Tile *tile = map.getForeground(l, j);
            if (!(tile -> isSolid || tile -> isPlatform)) {
                continue;
            }
            stays.y = j * TILE_HEIGHT + yOffset;
            // Cases where they start off colliding should be dealt with
            // elsewhere.
            if (stays.intersects(from)) {
                continue;
            }
            CollisionInfo info = findCollision(to, stays, dx, dy);
            // Include information about what tile was collided
            info.resolve(tile);
            // Add this collision to the list to deal with later
            // Also check whether any collision is inevitable
            if (info.type != CollisionType::NONE) {
                if (info.isInevitable) {
                    anyInevitable = true;
                }
                collisions.push_back(info);
            }
        }
    }
    return anyInevitable; 
}

//  A function that moves a movable to where it should end up on a map. This 
// assumes that no collisions with anything other than the map will affect the
// end location. It also assumes collisions between the very corner of the 
// movable and the very corner of a tile should be collisions in the x 
// direction. This is not always the case, so after calling this function once
// it's best to call it again with the results assuming the y position is 
// correct but the x may have farther to move.
void Collider::collide(const Map &map, Movable &movable) {
    // Calculate the world width and height
    int worldWidth = map.getWidth() * TILE_WIDTH;
    int worldHeight = map.getHeight() * TILE_HEIGHT;

    // from is the rectangle of the player the update before, to is the 
    // rectangle the player would move to if it didn't collide with anything,
    // and stays is the tile currently being checked for collisions with the
    // player.
    Rect from;
    Rect to;
    Rect stays;

    stays.w = TILE_WIDTH - 2 * xOffset;
    stays.h = TILE_HEIGHT - 2 * yOffset;

    // Tell all the Rects the world width, so they can wrap when checking for
    // collisions
    stays.worldWidth = worldWidth;
    from.worldWidth = worldWidth;
    to.worldWidth = worldWidth;

    // Move, collide, and stop at the edge of the map
    from.x = movable.x;
    from.y = movable.y;
    from.w = movable.spriteWidth;
    to.w = movable.spriteWidth;
    from.h = movable.spriteHeight;
    to.h = movable.spriteHeight;

    // Collide with tiles
    // Get basic information
    int width = movable.spriteWidth / TILE_WIDTH + 2;
    int height = movable.spriteHeight / TILE_HEIGHT + 2;
    int xVelocity = movable.getVelocity().x;
    int yVelocity = movable.getVelocity().y;
    int startX = from.x / TILE_WIDTH;
    int startY = from.y / TILE_HEIGHT; 
    // Collide with the tiles it starts on
    for (int k = startX; k < startX + width; k++) {
        int l = (k + map.getWidth()) % map.getWidth();
        stays.x = l * TILE_WIDTH + xOffset;
        for (int j = startY; j < startY + height; j++) {
            stays.y = j * TILE_HEIGHT + yOffset;
            if (stays.intersects(from) && enableCollisions) {
                // If I add sand that falls and does damage, I should 
                // deal that damage here.
                if (map.getForeground(l, j) -> isSolid) {
                    xVelocity = 0;
                    yVelocity = 0;
                }
            }
        }
    }

    // Collide with tiles it doesn't start on
    while (xVelocity != 0 || yVelocity != 0) {
        // The n is in case moludo results in 0 inconviniently
        int n = max(min(1, xVelocity), -1);
        int dx = (xVelocity - n) % (TILE_WIDTH / 2) + n;
        n = max(min(1, yVelocity), -1);
        int dy = (yVelocity - n) % (TILE_HEIGHT / 2) + n;
        to.x = from.x + dx;
        to.y = from.y + dy;
        int newX = from.x;
        int newY = from.y;
        // A separate variable so that corner collisions can only happen if
        // no other collisions happen.
        int cornerX = to.x;
        assert(abs(xVelocity - dx) <= abs(xVelocity));
        assert(abs(yVelocity - dy) <= abs(yVelocity));
        xVelocity -= dx;
        yVelocity -= dy;
        double xCoefficient = 1;
        double yCoefficient = 1;
        // Check whether there are and inevitable collisions we should 
        // handle first (they do need to be first, otherwise the player can
        // climb walls).
        bool anyInevitable = false;
        // Make a list of all collisions between the movable and any tiles 
        vector<CollisionInfo> collisions;
        anyInevitable = listCollisions(collisions, map, to, from);

        // Only collide if collisions are enabled
        while (enableCollisions && (collisions.size() != 0)) {
           // And actually handle the collisions
            for (unsigned int i = 0; i < collisions.size(); i++) {
                CollisionInfo info = collisions[i];
                // Skip this one if necessary
                if (anyInevitable && (!info.isInevitable)) {
                    continue;
                }
                xCoefficient *= info.xCoefficient;
                yCoefficient *= info.yCoefficient;
                if (info.newX != -1) {
                    newX = info.newX;
                    movable.isCollidingX = true;
                }
                if (info.type == CollisionType::DOWN) {
                    movable.isCollidingDown = true;
                }
                if (info.newY != -1) {
                    newY = info.newY;
                }
                if (info.cornerX != -1) {
                    cornerX = info.cornerX;
                }
            }

            // If there weren't any inevitable collisions, handle corner 
            // collisions and restart
            // Only check for corner collisions when there were no others
            bool usedCorner = false;
            if (!anyInevitable && xCoefficient != 0 && yCoefficient != 0) {
                movable.isCollidingX = true;
                newX = cornerX;
                xCoefficient = 0;
                usedCorner = true;
            }
 
            // Move the rest of the way
            dx = to.x - newX;
            dy = to.y - newY;
            dx *= xCoefficient;
            dy *= yCoefficient;
            xVelocity *= xCoefficient;
            yVelocity *= yCoefficient;

            // And repeat until we're done
            from.x = newX;
            from.y = newY;
            to.x = from.x + dx;
            to.y = from.y + dy;

           if (!anyInevitable && !usedCorner) {
                // This actually happens surprisingly rarely
                break;
            }
            else {
            }
            // Otherwise, check again for collisions.
            collisions.clear();
            anyInevitable = listCollisions(collisions, map, to, from);
        }

        // Set to where we're actaully moving to.
        from.x = to.x;
        from.y = to.y;
    }
    movable.x = from.x;
    movable.y = from.y;
    // Collide with the edge of the map
    // Wrap in the x direction
    movable.x += worldWidth;
    movable.x %= worldWidth;
    // Collide in the y direction
    movable.y = max(TILE_HEIGHT, movable.y);
    int worldTop = worldHeight - movable.spriteHeight - TILE_HEIGHT;
    movable.y = min(movable.y, worldTop);
}


// A function to move and collide the movables
// Note that this only ever resets distance fallen when it hits the ground.
void Collider::update(const Map &map, vector<Movable *> &movables) {
    // Update the velocity of everything
    for (unsigned i = 0; i < movables.size(); i++) {
        // If it fell, figure out how far
        if (movables[i] -> isCollidingDown) {
            int distanceFallen = movables[i] -> maxHeight - movables[i] -> y;
            movables[i] -> pixelsFallen = distanceFallen;
            movables[i] -> maxHeight = movables[i] -> y;
        }
        else {
            movables[i] -> pixelsFallen = 0;
        }

        // TODO: replace this with gravity as a function of height
        double gravity = -12;
        movables[i] -> gravity = gravity;
        movables[i] -> accelerate();
        movables[i] -> isSteppingUp = false;
        movables[i] -> isCollidingX = false;
        movables[i] -> isCollidingDown = false;

        // toX is the x value the movable expects to end up having.
        int toX = movables[i] -> x + movables[i] -> getVelocity().x;
        collide(map, *movables[i]);
        // Because collide() may stop things in the x direction before it 
        // should, we should try again.
        // Actually since after corner collisions it will just step up and 
        // contunue, this won't be a noticable bug most of the time, but might
        // as well fix it anyway.
        if (movables[i] -> isCollidingX) {
            // Have it move only the rest of the way in the x direction, now.
            Point newVelocity;
            newVelocity.x = toX - movables[i] -> x;
            newVelocity.y = 0;
            Point oldVelocity = movables[i] -> getVelocity();
            movables[i] -> setVelocity(newVelocity);
            collide(map, *movables[i]);
            movables[i] -> setVelocity(oldVelocity);
        }

        // Do the thing where colliding with a wall one block high doesn't
        // stop you
        if (movables[i] -> isCollidingX) {
            Movable hypothetical;
            hypothetical.x = movables[i] -> x;
            hypothetical.y = movables[i] -> y;
            hypothetical.spriteWidth = movables[i] -> spriteWidth;
            hypothetical.spriteHeight = movables[i] -> spriteHeight;
            // See if it can go up one tile or less without colliding
            int oldY = hypothetical.y;
            // The amount to move by to go up one tile or less, assuming 
            // gravity is in the usual direction
            int dy = TILE_HEIGHT - ((oldY + yOffset) % TILE_HEIGHT);
            assert(dy <= TILE_HEIGHT);
            assert(dy > 0);
            Point newVelocity;
            newVelocity.x = 0;
            newVelocity.y = dy;
            hypothetical.setVelocity(newVelocity);
            collide(map, hypothetical);
            // If there wasn't a collision
            if (hypothetical.y == oldY + dy) {
                assert(hypothetical.x == movables[i] -> x);
                assert(movables[i] -> y == oldY);
                // check that it would end up standing on the tile, and not 
                // randomly jump up and fall back down
                // dx is how much more it could have gone in the x direction, 
                // if it didn't collide with the tile it's stepping up
                int dx = toX - hypothetical.x;
                Point newVelocity;
                newVelocity.x = dx;
                newVelocity.y = 0;
                hypothetical.setVelocity(newVelocity);
                collide(map, hypothetical);
                if (hypothetical.x != movables[i] -> x) {
                    // Ok, so we do want to jump up and continue
                    // doing that instantaneously would look like:
                    // movables[i] -> x = hypothetical.x;
                    // movables[i] -> y = hypothetical.y;
                    // but we actually only want to go up by one x velocity
                    // (not one y velocity because the whole point of this is 
                    // that you go up without jumping).
                    if (dy < movables[i] -> getVelocity().x) {
                        movables[i] -> x = hypothetical.x;
                        movables[i] -> y = hypothetical.y;
                    }
                    else {
                        movables[i] -> y += abs(movables[i] -> getVelocity().x);
                        movables[i] -> isSteppingUp = true;
                    }
                }
            }
        }
        // And done checking whether it needs to step up.
        // Now we're in just the "for each movable" loop
        // Update the distance fallen from
        int newMaxHeight = max(movables[i] -> maxHeight, movables[i] -> y);
        movables[i] -> maxHeight = newMaxHeight;
    }
}

