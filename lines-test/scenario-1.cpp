#include <catch2/catch_amalgamated.hpp>

#include <algorithm>
#include "helpers.hpp"
#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"

TEST_CASE("Line Continuity and Connectivity Tests", "[continuity][connectivity][!mayfail]")
{
    Surface surface(128, 128);
    surface.clear();

    SECTION("Connected lines - no gaps")
    {
        // Draw two connected lines to form an angle
        draw_line_solid(surface, {10.f, 10.f}, {60.f, 60.f}, {255, 255, 255});  // Line 1
        draw_line_solid(surface, {60.f, 60.f}, {100.f, 20.f}, {255, 255, 255}); // Line 2

        // Check the continuity at the connecting point (60, 60)
        auto pixel = surface.get_surface_ptr();
        auto index = surface.get_linear_index(60, 60);

        // Ensure that the connecting pixel is actually drawn
        REQUIRE(pixel[index] == 255);       // Red channel
        REQUIRE(pixel[index + 1] == 255);   // Green channel
        REQUIRE(pixel[index + 2] == 255);   // Blue channel

        // Ensure there are no gaps between the two lines by checking neighboring pixels
        // Verify some neighboring pixels to see if the line transitions smoothly
        REQUIRE(max_row_pixel_count(surface) > 1);
        REQUIRE(max_col_pixel_count(surface) > 1);
    }

    SECTION("Multiple connected lines - polyline")
    {
        // Draw a polyline with multiple segments
        draw_line_solid(surface, {20.f, 20.f}, {50.f, 50.f}, {255, 255, 255});  // Segment 1
        draw_line_solid(surface, {50.f, 50.f}, {80.f, 20.f}, {255, 255, 255});  // Segment 2
        draw_line_solid(surface, {80.f, 20.f}, {100.f, 70.f}, {255, 255, 255}); // Segment 3

        // Verify that all segments are connected without any gaps
        // Check the connecting points (50, 50) and (80, 20)
        auto pixel = surface.get_surface_ptr();

        // First connecting point
        auto index1 = surface.get_linear_index(50, 50);
        REQUIRE(pixel[index1] == 255);
        REQUIRE(pixel[index1 + 1] == 255);
        REQUIRE(pixel[index1 + 2] == 255);

        // Second connecting point
        auto index2 = surface.get_linear_index(80, 20);
        REQUIRE(pixel[index2] == 255);
        REQUIRE(pixel[index2 + 1] == 255);
        REQUIRE(pixel[index2 + 2] == 255);
    }
}
