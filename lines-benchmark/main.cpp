#include <benchmark/benchmark.h>

#include "../draw2d/draw.hpp"
#include "../draw2d/draw-ex.hpp"
#include "../draw2d/surface-ex.hpp"

void draw_line_solid_dda(Surface &aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor)
{
    // Extract integer coordinates of start and end points
    int x0 = static_cast<int>(aBegin.x);
    int y0 = static_cast<int>(aBegin.y);
    int x1 = static_cast<int>(aEnd.x);
    int y1 = static_cast<int>(aEnd.y);

    // Get screen boundaries
    int width = aSurface.get_width();
    int height = aSurface.get_height();

    // Check if the entire line is outside the screen bounds
    if ((x0 < 0 && x1 < 0) || (x0 >= width && x1 >= width) ||
        (y0 < 0 && y1 < 0) || (y0 >= height && y1 >= height))
    {
        return; // Entire line is out of bounds, no need to draw
    }

    // Calculate dx and dy
    float dx = x1 - x0;
    float dy = y1 - y0;

    // Determine the number of steps required for the line
    int steps = std::max(std::abs(dx), std::abs(dy));

    // Calculate the increment for each step
    float xIncrement = dx / steps;
    float yIncrement = dy / steps;

    // Initialize starting position
    float x = x0;
    float y = y0;

    // Drawing loop with clipping
    for (int i = 0; i <= steps; ++i)
    {
        // Draw current pixel only if it's within bounds
        int xi = static_cast<int>(std::round(x));
        int yi = static_cast<int>(std::round(y));
        if (xi >= 0 && xi < width && yi >= 0 && yi < height)
        {
            aSurface.set_pixel_srgb(xi, yi, aColor);
        }

        // Increment x and y for the next step
        x += xIncrement;
        y += yIncrement;
    }
}

namespace
{
    void line_benchmark_bresenham_horizontal_(benchmark::State &aState)
    {
        auto const width = std::uint32_t(aState.range(0));
        auto const height = std::uint32_t(aState.range(1));

        SurfaceEx surface(width, height);
        surface.clear();

        Vec2f start{10, 50};
        Vec2f end{200, 50};

        for (auto _ : aState)
        {
            surface.clear();
            draw_line_solid(surface, start, end, ColorU8_sRGB{0, 0, 0});
            benchmark::ClobberMemory();
        }
    }

    void line_benchmark_bresenham_vertical_(benchmark::State &aState)
    {
        auto const width = std::uint32_t(aState.range(0));
        auto const height = std::uint32_t(aState.range(1));

        SurfaceEx surface(width, height);
        surface.clear();

        Vec2f start{50, 10};
        Vec2f end{50, 200};

        for (auto _ : aState)
        {
            surface.clear();
            draw_line_solid(surface, start, end, ColorU8_sRGB{0, 0, 0});
            benchmark::ClobberMemory();
        }
    }

    void line_benchmark_bresenham_diagonal_(benchmark::State &aState)
    {
        auto const width = std::uint32_t(aState.range(0));
        auto const height = std::uint32_t(aState.range(1));

        SurfaceEx surface(width, height);
        surface.clear();

        Vec2f start{10, 10};
        Vec2f end{200, 200};

        for (auto _ : aState)
        {
            surface.clear();
            draw_line_solid(surface, start, end, ColorU8_sRGB{0, 0, 0});
            benchmark::ClobberMemory();
        }
    }

    void line_benchmark_bresenham_steep_(benchmark::State &aState)
    {
        auto const width = std::uint32_t(aState.range(0));
        auto const height = std::uint32_t(aState.range(1));

        SurfaceEx surface(width, height);
        surface.clear();

        Vec2f start{10, 10};
        Vec2f end{30, 200};

        for (auto _ : aState)
        {
            surface.clear();
            draw_line_solid(surface, start, end, ColorU8_sRGB{0, 0, 0});
            benchmark::ClobberMemory();
        }
    }

    void line_benchmark_dda_horizontal_(benchmark::State &aState)
    {
        auto const width = std::uint32_t(aState.range(0));
        auto const height = std::uint32_t(aState.range(1));

        SurfaceEx surface(width, height);
        surface.clear();

        Vec2f start{10, 50};
        Vec2f end{200, 50};

        for (auto _ : aState)
        {
            surface.clear();
            draw_line_solid_dda(surface, start, end, ColorU8_sRGB{0, 255, 0});
            benchmark::ClobberMemory();
        }
    }

    void line_benchmark_dda_vertical_(benchmark::State &aState)
    {
        auto const width = std::uint32_t(aState.range(0));
        auto const height = std::uint32_t(aState.range(1));

        SurfaceEx surface(width, height);
        surface.clear();

        Vec2f start{50, 10};
        Vec2f end{50, 200};

        for (auto _ : aState)
        {
            surface.clear();
            draw_line_solid_dda(surface, start, end, ColorU8_sRGB{0, 255, 0});
            benchmark::ClobberMemory();
        }
    }

    void line_benchmark_dda_diagonal_(benchmark::State &aState)
    {
        auto const width = std::uint32_t(aState.range(0));
        auto const height = std::uint32_t(aState.range(1));

        SurfaceEx surface(width, height);
        surface.clear();

        Vec2f start{10, 10};
        Vec2f end{200, 200};

        for (auto _ : aState)
        {
            surface.clear();
            draw_line_solid_dda(surface, start, end, ColorU8_sRGB{0, 255, 0});
            benchmark::ClobberMemory();
        }
    }

    void line_benchmark_dda_steep_(benchmark::State &aState)
    {
        auto const width = std::uint32_t(aState.range(0));
        auto const height = std::uint32_t(aState.range(1));

        SurfaceEx surface(width, height);
        surface.clear();

        Vec2f start{10, 10};
        Vec2f end{30, 200};

        for (auto _ : aState)
        {
            surface.clear();
            draw_line_solid_dda(surface, start, end, ColorU8_sRGB{0, 255, 0});
            benchmark::ClobberMemory();
        }
    }
}

BENCHMARK(line_benchmark_bresenham_horizontal_)
    ->Args({320, 240})
    ->Args({1280, 720})
    ->Args({1920, 1080})
    ->Args({7680, 4320});

BENCHMARK(line_benchmark_bresenham_vertical_)
    ->Args({320, 240})
    ->Args({1280, 720})
    ->Args({1920, 1080})
    ->Args({7680, 4320});

BENCHMARK(line_benchmark_bresenham_diagonal_)
    ->Args({320, 240})
    ->Args({1280, 720})
    ->Args({1920, 1080})
    ->Args({7680, 4320});

BENCHMARK(line_benchmark_bresenham_steep_)
    ->Args({320, 240})
    ->Args({1280, 720})
    ->Args({1920, 1080})
    ->Args({7680, 4320});

BENCHMARK(line_benchmark_dda_horizontal_)
    ->Args({320, 240})
    ->Args({1280, 720})
    ->Args({1920, 1080})
    ->Args({7680, 4320});

BENCHMARK(line_benchmark_dda_vertical_)
    ->Args({320, 240})
    ->Args({1280, 720})
    ->Args({1920, 1080})
    ->Args({7680, 4320});

BENCHMARK(line_benchmark_dda_diagonal_)
    ->Args({320, 240})
    ->Args({1280, 720})
    ->Args({1920, 1080})
    ->Args({7680, 4320});

BENCHMARK(line_benchmark_dda_steep_)
    ->Args({320, 240})
    ->Args({1280, 720})
    ->Args({1920, 1080})
    ->Args({7680, 4320});

BENCHMARK_MAIN();
