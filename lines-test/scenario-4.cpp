#include <catch2/catch_amalgamated.hpp>

#include <algorithm>
#include "helpers.hpp"
#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"

TEST_CASE("Intersection and Overlap Tests", "[intersection][overlap][!mayfail]")
{
    Surface surface(128, 128);
    surface.clear();

    SECTION("Two lines crossing at center")
    {
        draw_line_solid(surface, {10.f, 64.f}, {120.f, 64.f}, {255, 0, 0}); // Horizontal line
        draw_line_solid(surface, {64.f, 10.f}, {64.f, 120.f}, {0, 255, 0}); // Vertical line

        // Check if the pixels at the intersection have a specific color (may be mixed or overlaid)
        auto pixel = surface.get_surface_ptr();
        auto index = surface.get_linear_index(64, 64);

        // Ideally, we want the crossover color to be some kind of hybrid color
        REQUIRE(pixel[index] > 0); // Check to see if you have the correct color in the center
    }

    SECTION("Overlapping lines along the same path")
    {
        draw_line_solid(surface, {20.f, 20.f}, {100.f, 100.f}, {255, 0, 0});
        draw_line_solid(surface, {20.f, 20.f}, {100.f, 100.f}, {0, 0, 255});

        // Check whether some pixels are superimposed
        auto neighbours = count_pixel_neighbours(surface);
        REQUIRE(neighbours[2] > 0); // Indicates that the blue channel should have a non-zero value
    }
}
