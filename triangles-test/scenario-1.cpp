#include <catch2/catch_amalgamated.hpp>

#include "helpers.hpp"
#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"

TEST_CASE("Triangle Continuity and Connectivity Tests", "[continuity][connectivity][!mayfail]")
{
    Surface surface(128, 128);
    surface.clear();

    SECTION("Solid Triangle - No gaps")
    {
        // Draw a solid triangle
        draw_triangle_solid(surface, {30.f, 30.f}, {80.f, 30.f}, {55.f, 80.f}, {255, 0, 0});  // Red triangle

        // Check if the triangle is drawn by checking some key pixels within the triangle
        auto pixel = surface.get_surface_ptr();
        auto index1 = surface.get_linear_index(55, 30); // Check a point near the base
        auto index2 = surface.get_linear_index(55, 60); // Check a point inside the triangle
        auto index3 = surface.get_linear_index(70, 70); // Check a point near the top

        // Ensure that these pixels are drawn in red (255, 0, 0)
        REQUIRE(pixel[index1] == 255);     // Red channel
        REQUIRE(pixel[index1 + 1] == 0);   // Green channel
        REQUIRE(pixel[index1 + 2] == 0);   // Blue channel

        REQUIRE(pixel[index2] == 255);     // Red channel
        REQUIRE(pixel[index2 + 1] == 0);   // Green channel
        REQUIRE(pixel[index2 + 2] == 0);   // Blue channel

        REQUIRE(pixel[index3] == 255);     // Red channel
        REQUIRE(pixel[index3 + 1] == 0);   // Green channel
        REQUIRE(pixel[index3 + 2] == 0);   // Blue channel
    }

    SECTION("Interpolated Triangle - Smooth color transition")
    {
        // Draw an interpolated triangle with color gradients
        draw_triangle_interp(
            surface, 
            {30.f, 30.f}, {80.f, 30.f}, {55.f, 80.f},
            {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}
        );  // Gradient from Red to Green to Blue

        // Check if color interpolation is correct by sampling several points
        auto pixel = surface.get_surface_ptr();
        auto index1 = surface.get_linear_index(55, 30); // Check a point near the base
        auto index2 = surface.get_linear_index(55, 60); // Check a point inside the triangle
        auto index3 = surface.get_linear_index(70, 70); // Check a point near the top

        // Verify color transitions at key points (approximate values for interpolated colors)
        REQUIRE(pixel[index1] == 255);      // Red channel (approximation)
        REQUIRE(pixel[index1 + 1] == 0);    // Green channel
        REQUIRE(pixel[index1 + 2] == 0);    // Blue channel

        REQUIRE(pixel[index2] == 127);      // Red channel (approximation)
        REQUIRE(pixel[index2 + 1] == 127);  // Green channel (approximation)
        REQUIRE(pixel[index2 + 2] == 0);    // Blue channel

        REQUIRE(pixel[index3] == 0);        // Red channel
        REQUIRE(pixel[index3 + 1] == 0);    // Green channel
        REQUIRE(pixel[index3 + 2] == 255);  // Blue channel
    }
}
