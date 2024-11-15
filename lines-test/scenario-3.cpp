#include <catch2/catch_amalgamated.hpp>

#include <algorithm>
#include "helpers.hpp"
#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"

TEST_CASE("Boundary Conditions Tests", "[boundary][!mayfail]")
{
    Surface surface(128, 128);
    surface.clear();

    SECTION("Draw line exactly on left boundary")
    {
        draw_line_solid(surface, {0.f, 10.f}, {0.f, 100.f}, {255, 255, 255});

        REQUIRE(max_col_pixel_count(surface) == 1);
        REQUIRE(max_row_pixel_count(surface) > 1);
    }

    SECTION("Draw line exactly on right boundary")
    {
        draw_line_solid(surface, {127.f, 20.f}, {127.f, 80.f}, {255, 255, 255});

        REQUIRE(max_col_pixel_count(surface) == 1);
        REQUIRE(max_row_pixel_count(surface) > 1);
    }

    SECTION("Draw line exactly on top boundary")
    {
        draw_line_solid(surface, {10.f, 0.f}, {120.f, 0.f}, {255, 255, 255});

        REQUIRE(max_row_pixel_count(surface) == 1);
        REQUIRE(max_col_pixel_count(surface) > 1);
    }

    SECTION("Draw line exactly on bottom boundary")
    {
        draw_line_solid(surface, {10.f, 127.f}, {120.f, 127.f}, {255, 255, 255});

        REQUIRE(max_row_pixel_count(surface) == 1);
        REQUIRE(max_col_pixel_count(surface) > 1);
    }
}

