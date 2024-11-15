#include <catch2/catch_amalgamated.hpp>
#include "helpers.hpp"
#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"

TEST_CASE("Edge Cases for Color Gradients", "[color][gradient][edge][triangle][!mayfail]")
{
    Surface surface(128, 128);
    surface.clear();

    SECTION("Solid Color Gradient - Red to Green")
    {
        // Draw a triangle with a simple solid gradient from red to green
        draw_triangle_interp(
            surface, 
            {30.f, 30.f}, {80.f, 30.f}, {55.f, 80.f},
            {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}  // Red to Green gradient
        );

        // Check gradient along the edges, from red to green
        auto pixel = surface.get_surface_ptr();

        // Bottom edge: Should gradually transition from red (255,0,0) to green (0,255,0)
        for (float x = 30.f; x <= 80.f; x++) {
            auto index = surface.get_linear_index(static_cast<int>(x), 30); // Bottom edge
            if (x < 55.f) {
                // Should be red at the left side of the edge
                REQUIRE(pixel[index] == 255);     // Red channel
                REQUIRE(pixel[index + 1] == 0);   // Green channel
                REQUIRE(pixel[index + 2] == 0);   // Blue channel
            } else {
                // Should be green at the right side of the edge
                REQUIRE(pixel[index] == 0);       // Red channel
                REQUIRE(pixel[index + 1] == 255); // Green channel
                REQUIRE(pixel[index + 2] == 0);   // Blue channel
            }
        }

        // Check the top vertex (55, 80), should be green
        auto index = surface.get_linear_index(55, 80);
        REQUIRE(pixel[index] == 0);           // Red channel
        REQUIRE(pixel[index + 1] == 255);     // Green channel
        REQUIRE(pixel[index + 2] == 0);       // Blue channel
    }
}
