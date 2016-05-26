#include "Map.hh"

using namespace std;

int main() {
    Map m = Map(WorldType::EARTH);
    m.save("map.bmp");
    return 0;
}
