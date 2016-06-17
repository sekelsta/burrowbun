#include "Map.hh"

using namespace std;

int main() {
//    Tile magma = Tile(TileType::MAGMA);
//    cout << int(magma.type) << endl;
    Map m = Map(WorldType::EARTH);
    m.save("map.world");
    Map t = Map(WorldType::TEST);
    t.save("test.world");
    return 0;
}
