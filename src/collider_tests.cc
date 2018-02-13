/* Make sure collision detection works properly. */

#define CATCH_CONFIG_MAIN // Tells catch to provide a main()
#include "catch.hpp"
#include "Collider.hh"

TEST_CASE("test Rect.intersects", "[intersects]") {
    Rect one;
    Rect two;
    one.worldWidth = 80;
    two.worldWidth = 80;

    /* Just a generic case of two rectangles. */
    SECTION("basic case") {
        one.w = 6;
        one.h = 8;
        two.w = 12;
        two.h = 10;
        one.y = 20;
        one.x = 5;

        SECTION("intersecting") {
            two.x = 6;
            two.y = 15;
            REQUIRE(one.intersects(two));
            REQUIRE(two.intersects(one));
        }

        SECTION("not intersecting") {
            two.x = 6;
            two.y = 32;
            REQUIRE_FALSE(one.intersects(two));
            REQUIRE_FALSE(two.intersects(one));
        }
    }

    /* Make sure adding worldWidth to x has no effect. */
    SECTION("wrapping rectangles") {
        int iterations = 1000;
        for (int i = 0; i < iterations; i++) {
            one.x = rand() % one.worldWidth;
            two.x = rand() % two.worldWidth;
            one.y = rand() % 40;
            two.y = rand() % 40;
            one.w = rand() % (one.worldWidth / 4) + 1;
            two.w = rand() % (two.worldWidth / 4) + 1;
            one.h = rand() % 20 + 1;
            two.h = rand() % 20 + 1;

            REQUIRE(one.intersects(two) == two.intersects(one));
            bool noWrapping = one.intersects(two);
            
            SECTION("wrapping around the large side") {
                one.x += one.worldWidth;
                bool wrapping = one.intersects(two);
                REQUIRE(noWrapping == wrapping);
            }

            SECTION("wrapping into the negatives") {
                one.x -= one.worldWidth;
                bool wrapping = one.intersects(two);
                REQUIRE(noWrapping == wrapping);
            }
        }
    }
}
