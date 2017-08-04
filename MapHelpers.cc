#include "MapHelpers.hh"

/* Return true if all values are the same. */
bool Location::operator==(const Location &location) {
    bool answer = location.x == this -> x;
    answer = answer && location.y == this -> y;
    return answer && location.layer == this -> layer;
}

