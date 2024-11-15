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

void draw_triangle_solid(Surface& aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorU8_sRGB aColor)
{
    // Step 1: Sort vertices by y-coordinate (ascending order)
    if (aP0.y > aP1.y) std::swap(aP0, aP1);
    if (aP0.y > aP2.y) std::swap(aP0, aP2);
    if (aP1.y > aP2.y) std::swap(aP1, aP2);

    // Step 2: Check for degenerate triangles
    if ((aP0.y == aP1.y && aP1.y == aP2.y) || (aP0.x == aP1.x && aP1.x == aP2.x)) {
        // All vertices are collinear, nothing to draw
        return;
    }

    // Step 3: Check for zero-area triangles
    int area = (aP1.x - aP0.x) * (aP2.y - aP0.y) - (aP2.x - aP0.x) * (aP1.y - aP0.y);
    if (std::abs(area) < 100) {
        return; // Area is too close to zero, skip drawing
    }

    int height = aSurface.get_height();
    int width = aSurface.get_width();

    // Step 4: Define a lambda function for drawing a scanline within bounds
    auto draw_scanline = [&](int y, int x_start, int x_end) {
        if (y < 0 || y >= height) return; // Skip out-of-bounds rows
        x_start = std::max(0, std::min(x_start, width - 1));
        x_end = std::max(0, std::min(x_end, width - 1));
        for (int x = x_start; x <= x_end; ++x) {
            aSurface.set_pixel_srgb(x, y, aColor);
        }
    };

    // Step 5: Define a lambda function for linear interpolation of x based on y (using integer arithmetic)
    auto interpolate_x = [](int y, Vec2f p0, Vec2f p1) -> int {
        if (p0.y == p1.y) return static_cast<int>(p0.x); // Avoid division by zero for horizontal lines
        return static_cast<int>(p0.x + (y - p0.y) * (p1.x - p0.x) / (p1.y - p0.y));
    };

    // Step 6: Handle the whole triangle with a single loop
    int y_start = static_cast<int>(aP0.y);
    int y_end = static_cast<int>(aP2.y);
    
    for (int y = y_start; y <= y_end; ++y) {
        int x_start, x_end;

        if (y < static_cast<int>(aP1.y)) {
            // Lower part (from aP0 to aP1)
            x_start = interpolate_x(y, aP0, aP2);
            x_end = interpolate_x(y, aP0, aP1);
        } else {
            // Upper part (from aP1 to aP2)
            x_start = interpolate_x(y, aP0, aP2);
            x_end = interpolate_x(y, aP1, aP2);
        }

        // Ensure x_start <= x_end
        if (x_start > x_end) std::swap(x_start, x_end);

        draw_scanline(y, x_start, x_end);
    }
}


void draw_triangle_interp(Surface &aSurface, Vec2f aP0, Vec2f aP1, Vec2f aP2, ColorF aC0, ColorF aC1, ColorF aC2)
{
    if (aP0.y > aP1.y) std::swap(aP0, aP1);
    if (aP0.y > aP2.y) std::swap(aP0, aP2);
    if (aP1.y > aP2.y) std::swap(aP1, aP2);

    int height = aSurface.get_height();
    int width = aSurface.get_width();

    auto linear_to_srgb = [](float linear) -> uint8_t {
        return (linear <= 0.0031308f) ? 
            static_cast<uint8_t>(std::round(linear * 12.92f * 255.f)) : 
            static_cast<uint8_t>(std::round((1.055f * std::pow(linear, 1.f / 2.4f) - 0.055f) * 255.f));
    };

    auto barycentric_weights = [&](Vec2f p) -> Vec2f {
        float denom = (aP1.y - aP2.y) * (aP0.x - aP2.x) + (aP2.x - aP1.x) * (aP0.y - aP2.y);
        Vec2f weights;
        weights.x = ((aP1.y - aP2.y) * (p.x - aP2.x) + (aP2.x - aP1.x) * (p.y - aP2.y)) / denom;
        weights.y = ((aP2.y - aP0.y) * (p.x - aP2.x) + (aP0.x - aP2.x) * (p.y - aP2.y)) / denom;
        return weights;
    };

    for (int y = static_cast<int>(std::floor(aP0.y)); y <= static_cast<int>(std::ceil(aP2.y)); ++y) {
        for (int x = static_cast<int>(std::floor(std::min({aP0.x, aP1.x, aP2.x}))); x <= static_cast<int>(std::ceil(std::max({aP0.x, aP1.x, aP2.x}))); ++x) {
            if (x < 0 || x >= width || y < 0 || y >= height) continue;

            Vec2f weights = barycentric_weights(Vec2f{ static_cast<float>(x), static_cast<float>(y) });
            float w2 = 1.0f - weights.x - weights.y;

            if (weights.x >= 0 && weights.y >= 0 && w2 >= 0) {
                ColorF interpolatedColor = {
                    weights.x * aC0.r + weights.y * aC1.r + w2 * aC2.r,
                    weights.x * aC0.g + weights.y * aC1.g + w2 * aC2.g,
                    weights.x * aC0.b + weights.y * aC1.b + w2 * aC2.b
                };

                ColorU8_sRGB finalColor{
                    linear_to_srgb(interpolatedColor.r),
                    linear_to_srgb(interpolatedColor.g),
                    linear_to_srgb(interpolatedColor.b)
                };

                aSurface.set_pixel_srgb(x, y, finalColor);
            }
        }
    }
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
