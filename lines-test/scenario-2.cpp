#include <catch2/catch_amalgamated.hpp>

#include <algorithm>
#include "helpers.hpp"
#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"

// Pixel Distribution Tests
TEST_CASE("Pixel Distribution Tests", "[pixel_distribution][!mayfail]")
{
    // Set up the surface for each test
    Surface surface(128, 128);
    surface.clear();

    SECTION("Horizontal line")
    {
        draw_line_solid(surface,
            {10.f, 47.f},
            {110.f, 47.f},
            {255, 255, 255}
        );

        // Check the pixel distribution along the row and column
        REQUIRE(max_row_pixel_count(surface) > 1);
        REQUIRE(1 == max_col_pixel_count(surface));
    }

    SECTION("Vertical line")
    {
        draw_line_solid(surface,
            {64.f, 28.f},
            {64.f, 100.f},
            {255, 255, 255}
        );

        // Check the pixel distribution along the row and column
        REQUIRE(1 == max_row_pixel_count(surface));
        REQUIRE(max_col_pixel_count(surface) > 1);
    }

    SECTION("Diagonal line")
    {
        draw_line_solid(surface,
            {10.f, 10.f},
            {100.f, 100.f},
            {255, 255, 255}
        );

        // Check the pixel distribution for diagonal line
        REQUIRE(1 == max_row_pixel_count(surface));
        REQUIRE(1 == max_col_pixel_count(surface));
    }

    SECTION("X-major line")
    {
        draw_line_solid(surface,
            {1.f, 20.f},
            {127.f, 60.f},
            {255, 255, 255}
        );

        // Check the pixel distribution along the row and column
        REQUIRE(max_row_pixel_count(surface) > 1);
        REQUIRE(1 == max_col_pixel_count(surface));
    }

    SECTION("Y-major line")
    {
        draw_line_solid(surface,
            {79.f, 28.f},
            {70.f, 100.f},
            {255, 255, 255}
        );

        // Check the pixel distribution along the row and column
        REQUIRE(1 == max_row_pixel_count(surface));
        REQUIRE(max_col_pixel_count(surface) > 1);
    }

    SECTION("Line with varying slope")
    {
        draw_line_solid(surface,
            {10.f, 20.f},
            {110.f, 100.f},
            {255, 255, 255}
        );

        // Check the pixel distribution for the line with varying slope
        REQUIRE(max_row_pixel_count(surface) > 1);
        REQUIRE(max_col_pixel_count(surface) > 1);
    }
}
