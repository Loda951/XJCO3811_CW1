#include <catch2/catch_amalgamated.hpp>

#include <algorithm>
#include "helpers.hpp"
#include "../draw2d/surface.hpp"
#include "../draw2d/draw.hpp"

#include <chrono>

TEST_CASE("Performance and Efficiency Tests", "[performance]")
{
    Surface surface(1024, 1024);
    surface.clear();

    SECTION("Draw large number of lines within time limit")
    {
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < 10000; ++i)
        {
            draw_line_solid(surface,
                {static_cast<float>(rand() % 1024), static_cast<float>(rand() % 1024)},
                {static_cast<float>(rand() % 1024), static_cast<float>(rand() % 1024)},
                {255, 255, 255});
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        REQUIRE(elapsed.count() < 1.0); // Check that all draws are completed within 1 second
    }
}
