#include "MapHelpers.hh"

/* Return true if all values are the same. */
bool Location::operator==(const Location &location) const {
    bool answer = location.x == this -> x;
    answer = answer && location.y == this -> y;
    return answer && location.layer == this -> layer;
}

bool Location::operator!=(const Location &location) const {
    return !(*this == location);
}

/* Return true if this Location is smaller. Smaller in this case means it
has a larger y value, or, if y values are the same, a smaller x value. */
bool Location::operator<(const Location &location) const {
    /* If the y values aren't equal, sort by y value. */
    if (this -> y != location.y) {
        return this -> y > location.y;
    }

    /* If the x values aren't equal, sort by x value. */
    if (this -> x != location.x) {
        return this -> x < location.x;
    }

    /* Otherwise sort by layer. */
    return (int)(this -> layer) < (int)(location.layer);
}
