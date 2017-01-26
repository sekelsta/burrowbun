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
        int w, const Rect &stays) {
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
        int h, const Rect &stays) {
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
        int dx, int dy) {
    CollisionInfo info;
    info.x = 0;
    info.y = 0;

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
            return info;
        }
        // Now we know none of the edges started off in the collision area,
        // so since we know there is a collision, there must be at least two
        // edges in the collision area.
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

//  A function that moves a movable to where it should end up on a map. This 
// assumes that no collisions with anything other than the map will affect the
// end location.
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

    // In case these were true before
    movable.isCollidingX = false;
    movable.isCollidingDown = false;
 

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
    // Calculating this here instead of with the other things, 
    // in case velocity changes after start collisions
    while (xVelocity != 0 || yVelocity != 0) {
        // The n is in case moludo results in 0 inconviniently
        int n = max(min(1, xVelocity), -1);
        int dx = (xVelocity - n) % (TILE_WIDTH / 2) + n;
        n = max(min(1, yVelocity), -1);
        int dy = (yVelocity - n) % (TILE_HEIGHT / 2) + n;
        to.x = from.x + dx;
        to.y = from.y + dy;
        int newX = to.x;
        int newY = to.y;
        // A separate variable so that corner collisions can only happen if
        // no other collisions happen.
        int cornerX = to.x;
        assert(abs(xVelocity - dx) <= abs(xVelocity));
        assert(abs(yVelocity - dy) <= abs(yVelocity));
        xVelocity -= dx;
        yVelocity -= dy;
        double xCoefficient = 1;
        double yCoefficient = 1;
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
                if (stays.intersects(from)) {
                    continue;
                }
                CollisionInfo info = findCollision(to, stays, dx, dy);
                switch(info.type) {
                    case CollisionType::DOWN :
                        movable.isCollidingDown = true;
                        yCoefficient *= (int)(!(tile -> isPlatform));
                    case CollisionType::UP :
                        newY = info.y;
                        yCoefficient *= (int)(!(tile -> isSolid));
                        break;
                    case CollisionType::LEFT :
                    case CollisionType::RIGHT :
                        newX = info.x;
                        if (tile -> isSolid) {
                            xCoefficient = 0;
                            movable.isCollidingX = true;
                        }
                        break;
                    case CollisionType::LEFT_CORNER :
                    case CollisionType::RIGHT_CORNER :
                        if (tile -> isSolid) {
                            cornerX = info.x;
                        }
                        break;
                    case CollisionType::NONE :
                        break;
                }
            }
        }
        // Back in the while, move loop
        // In case the collisions doesn't stop us
        dx = to.x - newX;
        dy = to.y - newY;

        // Only collide if collisions are enabled
        if (enableCollisions) {
            dx *= xCoefficient;
            dy *= yCoefficient;
            xVelocity *= xCoefficient;
            yVelocity *= yCoefficient;
        }
        // This will only work if there aren't half-tiles
        from.x = newX + dx;
        from.y = newY + dy;
        // Handle corner collisions if necessary
        if (from.x == to.x && from.y == to.y) {
            from.x = cornerX;
        }
    }
    movable.x = from.x;
    movable.y = from.y;
    // Collide with the edge of the map
    // Wrap in the x direction
    movable.x += worldWidth;
    movable.x %= worldWidth;
    // Collide in the y direction
    movable.y = max(0, movable.y);
    movable.y = min(movable.y, worldHeight - movable.spriteHeight);
}


// A function to move and collide the movables
void Collider::update(const Map &map, vector<Movable *> &movables) {
    // Update the velocity of everything
    for (unsigned i = 0; i < movables.size(); i++) {
        movables[i] -> accelerate();
        // This should be changed
        double gravity = -5;
        Point v = movables[i] -> getVelocity();
        v.y += gravity;
        movables[i] -> setVelocity(v);

        int oldX = movables[i] -> x;
        collide(map, *movables[i]);
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
                int dx = oldX + movables[i] -> getVelocity().x;
                dx -= hypothetical.x;
                assert(hypothetical.x + dx == oldX + movables[i] -> getVelocity().x);
                Point newVelocity;
                newVelocity.x = dx;
                newVelocity.y = 0;
                hypothetical.setVelocity(newVelocity);
                collide(map, hypothetical);
                if (hypothetical.x != movables[i] -> x) {
                    // Ok, so we do want to jump up and continue
                    movables[i] -> x = hypothetical.x;
                    movables[i] -> y = hypothetical.y;
                }
            }
        }
    }
}

