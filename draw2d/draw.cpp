#include "draw.hpp"

#include <algorithm>

#include <cmath>

#include "surface.hpp"

void draw_line_solid(Surface &aSurface, Vec2f aBegin, Vec2f aEnd, ColorU8_sRGB aColor)
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

	// Bresenham's algorithm variables
	int dx = std::abs(x1 - x0);
	int dy = std::abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int err = dx - dy;

	// Drawing loop with clipping
	while (true)
	{
		// Draw current pixel only if it's within bounds
		if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
		{
			aSurface.set_pixel_srgb(x0, y0, aColor);
		}

		// Check if we've reached the endpoint
		if (x0 == x1 && y0 == y1)
			break;

		// Update errors and coordinates
		int e2 = err * 2;
		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}

void draw_triangle_wireframe(Surface &aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorU8_sRGB aColor)
{
	// TODO: your implementation goes here
	// TODO: your implementation goes here
	// TODO: your implementation goes here

	// TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments until the function
	(void)aP0;		// is properly implemented.
	(void)aP1;
	(void)aP2;
	(void)aColor;
}

void draw_triangle_solid(Surface &aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorU8_sRGB aColor)
{
    // Step 1: Sort vertices by y-coordinate (ascending order)
    if (aP0.y > aP1.y) std::swap(aP0, aP1);
    if (aP0.y > aP2.y) std::swap(aP0, aP2);
    if (aP1.y > aP2.y) std::swap(aP1, aP2);

    // Step 2: Screen bounds
    int width = aSurface.get_width();
    int height = aSurface.get_height();

    // Clipping all points to screen bounds
    auto clip_x = [&](float x)
    { return std::max(7.0f, std::min(x, static_cast<float>(width - 5))); };
    auto clip_y = [&](float y)
    { return std::max(6.0f, std::min(y, static_cast<float>(height - 5))); };

    aP0.x = clip_x(aP0.x); aP0.y = clip_y(aP0.y);
    aP1.x = clip_x(aP1.x); aP1.y = clip_y(aP1.y);
    aP2.x = clip_x(aP2.x); aP2.y = clip_y(aP2.y);

    // Step 3: Check for degenerate triangles
    if ((aP0.y == aP1.y && aP1.y == aP2.y) || (aP0.x == aP1.x && aP1.x == aP2.x))
        return;

    // Step 4: Check for zero-area triangles
    float area = (aP1.x - aP0.x) * (aP2.y - aP0.y) - (aP2.x - aP0.x) * (aP1.y - aP0.y);
    if (std::abs(area) < 1e-6) return;

    // Step 5: Define a lambda function for drawing a scanline within bounds
    auto draw_scanline = [&](int y, int x_start, int x_end)
    {
        x_start = std::max(0, std::min(x_start, width - 1));
        x_end = std::max(0, std::min(x_end, width - 1));
        if (x_start > x_end) std::swap(x_start, x_end);
        for (int x = x_start; x <= x_end; ++x)
        {
            aSurface.set_pixel_srgb(x, y, aColor);
        }
    };

    // Optimization: Precompute inverse slopes
    float dxdy_p0p1 = 0.0f, dxdy_p0p2 = 0.0f, dxdy_p1p2 = 0.0f;
    if (aP1.y - aP0.y != 0) dxdy_p0p1 = (aP1.x - aP0.x) / (aP1.y - aP0.y);
    if (aP2.y - aP0.y != 0) dxdy_p0p2 = (aP2.x - aP0.x) / (aP2.y - aP0.y);
    if (aP2.y - aP1.y != 0) dxdy_p1p2 = (aP2.x - aP1.x) / (aP2.y - aP1.y);

    // Decide if triangle is flat-top, flat-bottom, or needs to be split
    if (aP1.y == aP0.y) // Flat-top
    {
        if (aP1.x < aP0.x) std::swap(aP0, aP1);
        float yStart = std::ceil(aP0.y);
        float yEnd = std::floor(aP2.y);

        float xLeft = aP0.x + (yStart - aP0.y) * dxdy_p0p2;
        float xRight = aP1.x + (yStart - aP1.y) * dxdy_p1p2;

        for (int y = static_cast<int>(yStart); y <= static_cast<int>(yEnd); ++y)
        {
            draw_scanline(y, static_cast<int>(xLeft), static_cast<int>(xRight));
            xLeft += dxdy_p0p2;
            xRight += dxdy_p1p2;
        }
    }
    else if (aP1.y == aP2.y) // Flat-bottom
    {
        if (aP2.x < aP1.x) std::swap(aP1, aP2);
        float yStart = std::ceil(aP0.y);
        float yEnd = std::floor(aP1.y);

        float xLeft = aP0.x + (yStart - aP0.y) * dxdy_p0p1;
        float xRight = aP0.x + (yStart - aP0.y) * dxdy_p0p2;

        for (int y = static_cast<int>(yStart); y <= static_cast<int>(yEnd); ++y)
        {
            draw_scanline(y, static_cast<int>(xLeft), static_cast<int>(xRight));
            xLeft += dxdy_p0p1;
            xRight += dxdy_p0p2;
        }
    }
    else // General triangle
    {
        // Split the triangle into a flat-bottom and a flat-top triangle
        float alpha = (aP1.y - aP0.y) / (aP2.y - aP0.y);
        float newX = aP0.x + alpha * (aP2.x - aP0.x);
        Vec2f aP3(newX, aP1.y);

        // First half (flat-bottom)
        {
            float yStart = std::ceil(aP0.y);
            float yEnd = std::floor(aP1.y) - 1;

            float xLeft = aP0.x + (yStart - aP0.y) * dxdy_p0p1;
            float xRight = aP0.x + (yStart - aP0.y) * dxdy_p0p2;

            for (int y = static_cast<int>(yStart); y <= static_cast<int>(yEnd); ++y)
            {
                draw_scanline(y, static_cast<int>(xLeft), static_cast<int>(xRight));
                xLeft += dxdy_p0p1;
                xRight += dxdy_p0p2;
            }
        }

        // Second half (flat-top)
        {
            float yStart = std::ceil(aP1.y);
            float yEnd = std::floor(aP2.y);

            float xLeft = aP1.x + (yStart - aP1.y) * dxdy_p1p2;
            float xRight = aP3.x + (yStart - aP1.y) * dxdy_p0p2;

            for (int y = static_cast<int>(yStart); y <= static_cast<int>(yEnd); ++y)
            {
                draw_scanline(y, static_cast<int>(xLeft), static_cast<int>(xRight));
                xLeft += dxdy_p1p2;
                xRight += dxdy_p0p2;
            }
        }
    }
}


void draw_triangle_interp(Surface &aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorF aC0, ColorF aC1, ColorF aC2)
{
	// TODO: your implementation goes here
	// TODO: your implementation goes here
	// TODO: your implementation goes here

	// TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments until the function
	(void)aP0;		// is properly implemented.
	(void)aP1;
	(void)aP2;
	(void)aC0;
	(void)aC1;
	(void)aC2;
}

void draw_rectangle_solid(Surface &aSurface, Vec2f aMinCorner, Vec2f aMaxCorner, ColorU8_sRGB aColor)
{
	// TODO: your implementation goes here
	// TODO: your implementation goes here
	// TODO: your implementation goes here

	// TODO: remove the following when you start your implementation
	(void)aSurface;	  // Avoid warnings about unused arguments until the function
	(void)aMinCorner; // is properly implemented.
	(void)aMaxCorner;
	(void)aColor;
}

void draw_rectangle_outline(Surface &aSurface, Vec2f aMinCorner, Vec2f aMaxCorner, ColorU8_sRGB aColor)
{
	// TODO: your implementation goes here
	// TODO: your implementation goes here
	// TODO: your implementation goes here

	// TODO: remove the following when you start your implementation
	(void)aSurface; // Avoid warnings about unused arguments
	(void)aMinCorner;
	(void)aMaxCorner;
	(void)aColor;
}
