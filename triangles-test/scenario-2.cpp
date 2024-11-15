#include <catch2/catch_amalgamated.hpp>
#include "helpers.hpp"
#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"

TEST_CASE("Triangle Edge Pixel Continuity Tests", "[continuity][triangle][edge][!mayfail]")
{
    Surface surface(128, 128);
    surface.clear();

    SECTION("Solid Triangle - Edge Pixel Continuity")
    {
        // Draw a solid triangle
        draw_triangle_solid(surface, {30.f, 30.f}, {80.f, 30.f}, {55.f, 80.f}, {255, 0, 0});  // Red triangle

        // Check edge pixels to ensure no gaps on the boundaries
        auto pixel = surface.get_surface_ptr();

        // Check points along the edges of the triangle
        for (float x = 30.f; x <= 80.f; x++) {
            auto index = surface.get_linear_index(static_cast<int>(x), 30); // Bottom edge
            REQUIRE(pixel[index] == 255);     // Red channel
            REQUIRE(pixel[index + 1] == 0);   // Green channel
            REQUIRE(pixel[index + 2] == 0);   // Blue channel
        }

        for (float y = 30.f; y <= 80.f; y++) {
            auto index = surface.get_linear_index(80, static_cast<int>(y)); // Right edge
            REQUIRE(pixel[index] == 255);     // Red channel
            REQUIRE(pixel[index + 1] == 0);   // Green channel
            REQUIRE(pixel[index + 2] == 0);   // Blue channel
        }

        for (float y = 30.f; y <= 80.f; y++) {
            auto index = surface.get_linear_index(static_cast<int>(55.f), static_cast<int>(y)); // Left edge
            REQUIRE(pixel[index] == 255);     // Red channel
            REQUIRE(pixel[index + 1] == 0);   // Green channel
            REQUIRE(pixel[index + 2] == 0);   // Blue channel
        }

        // Check the corner pixel (55, 80) to ensure it's filled as part of the triangle
        auto index = surface.get_linear_index(55, 80);
        REQUIRE(pixel[index] == 255);         // Red channel
        REQUIRE(pixel[index + 1] == 0);       // Green channel
        REQUIRE(pixel[index + 2] == 0);       // Blue channel
    }

    SECTION("Interpolated Triangle - Edge Pixel Continuity")
    {
        // Draw an interpolated triangle with color gradients
        draw_triangle_interp(
            surface, 
            {30.f, 30.f}, {80.f, 30.f}, {55.f, 80.f},
            {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}
        );  // Gradient from Red to Green to Blue

        // Check edge pixels to ensure smooth gradient continuity along the boundaries
        auto pixel = surface.get_surface_ptr();

        // Check points along the edges of the triangle (with interpolated colors)
        for (float x = 30.f; x <= 80.f; x++) {
            auto index = surface.get_linear_index(static_cast<int>(x), 30); // Bottom edge
            REQUIRE(pixel[index] == 255);     // Red channel
            REQUIRE(pixel[index + 1] == 0);   // Green channel
            REQUIRE(pixel[index + 2] == 0);   // Blue channel
        }

        for (float y = 30.f; y <= 80.f; y++) {
            auto index = surface.get_linear_index(80, static_cast<int>(y)); // Right edge
            REQUIRE(pixel[index] == 0);       // Blue channel at the top
            REQUIRE(pixel[index + 1] == 0);   // Green channel
            REQUIRE(pixel[index + 2] == 255); // Blue channel
        }

        for (float y = 30.f; y <= 80.f; y++) {
            auto index = surface.get_linear_index(static_cast<int>(55.f), static_cast<int>(y)); // Left edge
            REQUIRE(pixel[index] == 0);       // Green channel at the top
            REQUIRE(pixel[index + 1] == 255); // Green channel
            REQUIRE(pixel[index + 2] == 0);   // Blue channel
        }

        // Check the corner pixel (55, 80) to ensure smooth interpolation at the top
        auto index = surface.get_linear_index(55, 80);
        REQUIRE(pixel[index] == 0);           // Green channel
        REQUIRE(pixel[index + 1] == 0);       // Green channel
        REQUIRE(pixel[index + 2] == 255);     // Blue channel
    }
}
