#include <benchmark/benchmark.h>

#include <algorithm>

#include <cassert>

#include "../draw2d/image.hpp"
#include "../draw2d/draw-ex.hpp"
#include "../draw2d/surface-ex.hpp"

namespace
{
	void default_blit_earth_(benchmark::State &aState)
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));

		SurfaceEx surface(width, height);
		surface.clear();

		auto source = load_image("assets/earth.png");
		assert(source);

		for (auto _ : aState)
		{
			// Make sure as much as possible of the image is visible. The
			// sample image here is 1k*1k. Blitting to center = (500,500) makes
			// sure that as much as possible of the image ends up in the target
			// surface.
			blit_masked(surface, *source, {500.f, 500.f});

			// ClobberMemory() ensures that the compiler won't optimize away
			// our blit operation. (Unlikely, but technically poossible.)
			benchmark::ClobberMemory();
		}

		// The following enables the benchmarking library to print information
		// about the memory bandwidth. The total number of bytes processed is
		// *approximatively* two times the total number of bytes in the blit,
		// accounding for both reading and writing. ("Approximatively" since
		// not all pixels are written.)
		auto const maxBlitX = std::min(width, source->get_width());
		auto const maxBlitY = std::min(height, source->get_height());

		aState.SetBytesProcessed(2 * maxBlitX * maxBlitY * 4 * aState.iterations());
	}

	void blit_earth_without_alpha_(benchmark::State &aState)
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));

		SurfaceEx surface(width, height);
		surface.clear();

		// 加载源图像（假设 "assets/earth.png" 存在）
		auto source = load_image("assets/earth.png");
		assert(source);

		// 目标图像的起始位置
		int startX = 500;
		int startY = 500;

		for (auto _ : aState)
		{
			for (int y = 0; y < source->get_height(); ++y)
			{
				for (int x = 0; x < source->get_width(); ++x)
				{
					int destX = startX + x;
					int destY = startY + y;

					// 检查目标像素位置是否在帧缓冲区范围内
					if (destX >= 0 && destX < width && destY >= 0 && destY < height)
					{
						// 获取源图像中的像素，但忽略 Alpha 通道
						ColorU8_sRGB_Alpha color = source->get_pixel(x, y);
						surface.set_pixel_srgb(destX, destY, {color.r, color.g, color.b});
					}
				}
			}
			benchmark::ClobberMemory(); // 防止编译器优化掉 `blit` 操作
		}

		// 计算总处理字节数，忽略 Alpha 通道（只计算 RGB）
		auto const maxBlitX = std::min(width, source->get_width());
		auto const maxBlitY = std::min(height, source->get_height());
		aState.SetBytesProcessed(3 * maxBlitX * maxBlitY * aState.iterations()); // 仅 RGB 通道
	}

	void blit_earth_with_memcpy_(benchmark::State &aState)
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));

		SurfaceEx surface(width, height);
		surface.clear();

		// 加载源图像（假设 "assets/earth.png" 存在）
		auto source = load_image("assets/earth.png");
		assert(source);

		// 目标图像的起始位置
		int startX = 500;
		int startY = 500;

		// 获取源图像的宽度和高度
		std::uint32_t imageWidth = source->get_width();
		std::uint32_t imageHeight = source->get_height();

		for (auto _ : aState)
		{
			for (std::uint32_t y = 0; y < imageHeight; ++y)
			{
				int destY = startY + y;
				if (destY >= 0 && destY < height)
				{																									 // 确保在目标范围内
					const std::uint8_t *srcRow = source->get_image_ptr() + y * imageWidth * 4;						 // 源图像行起始地址
					std::uint8_t *destRow = surface.get_surface_ptr() + surface.get_linear_index(startX, destY) * 4; // 目标行起始地址

					// 使用 std::memcpy 复制整行 RGB 数据到目标帧缓冲区
					std::memcpy(destRow, srcRow, imageWidth * 4);
				}
			}
			benchmark::ClobberMemory(); // 防止编译器优化
		}

		// 计算总处理字节数
		auto const maxBlitX = std::min(width, imageWidth);
		auto const maxBlitY = std::min(height, imageHeight);
		aState.SetBytesProcessed(2 * maxBlitX * maxBlitY * 4 * aState.iterations()); // 计入 RGBx (32-bit) 数据
	}

	void blit_without_alpha_(benchmark::State &aState, std::uint32_t imageWidth, std::uint32_t imageHeight)
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));

		SurfaceEx surface(width, height);
		surface.clear();

		// 创建虚拟的源图像数据
		std::unique_ptr<std::uint8_t[]> sourceData(new std::uint8_t[imageWidth * imageHeight * 4]());
		for (std::uint32_t i = 0; i < imageWidth * imageHeight * 4; ++i)
		{
			sourceData[i] = static_cast<std::uint8_t>(rand() % 256);
		}

		// 起始位置
		int startX = 500;
		int startY = 500;

		for (auto _ : aState)
		{
			for (std::uint32_t y = 0; y < imageHeight; ++y)
			{
				const std::uint8_t *srcRow = sourceData.get() + y * imageWidth * 4;
				int destY = startY + y;
				if (destY >= 0 && destY < height)
				{ // 确保在目标范围内
					for (std::uint32_t x = 0; x < imageWidth; ++x)
					{
						int destX = startX + x;
						if (destX >= 0 && destX < width)
						{ // 确保在帧缓冲区范围内
							std::uint8_t r = srcRow[x * 4];
							std::uint8_t g = srcRow[x * 4 + 1];
							std::uint8_t b = srcRow[x * 4 + 2];
							// 将 RGB 值直接复制到目标 surface，不考虑 Alpha 值
							surface.set_pixel_srgb(destX, destY, {r, g, b});
						}
					}
				}
			}
			benchmark::ClobberMemory(); // 防止编译器优化
		}

		auto const maxBlitX = std::min(width, imageWidth);
		auto const maxBlitY = std::min(height, imageHeight);
		aState.SetBytesProcessed(3 * maxBlitX * maxBlitY * aState.iterations()); // 仅 RGB 通道
	}

	void blit_with_memcpy_(benchmark::State &aState, std::uint32_t imageWidth, std::uint32_t imageHeight)
	{
		auto const width = std::uint32_t(aState.range(0));
		auto const height = std::uint32_t(aState.range(1));

		SurfaceEx surface(width, height);
		surface.clear();

		// 在指定尺寸创建一个虚拟的源图像数据
		std::unique_ptr<std::uint8_t[]> sourceData(new std::uint8_t[imageWidth * imageHeight * 4]());
		// 初始化图像数据（假设颜色随机）
		for (std::uint32_t i = 0; i < imageWidth * imageHeight * 4; ++i)
		{
			sourceData[i] = static_cast<std::uint8_t>(rand() % 256);
		}

		// 起始位置
		int startX = 500;
		int startY = 500;

		for (auto _ : aState)
		{
			// 每次迭代执行 blit 操作
			for (std::uint32_t y = 0; y < imageHeight; ++y)
			{
				const std::uint8_t *srcRow = sourceData.get() + y * imageWidth * 4;
				int destY = startY + y;
				if (destY >= 0 && destY < height)
				{ // 确保在目标范围内
					std::uint8_t *destRow = surface.get_surface_ptr() + surface.get_linear_index(startX, destY) * 4;
					std::memcpy(destRow, srcRow, imageWidth * 4); // 将一行图像复制到目标表面
				}
			}
			benchmark::ClobberMemory(); // 防止编译器优化
		}

		// 设置处理的字节数
		auto const maxBlitX = std::min(width, imageWidth);
		auto const maxBlitY = std::min(height, imageHeight);
		aState.SetBytesProcessed(2 * maxBlitX * maxBlitY * 4 * aState.iterations());
	}
}

BENCHMARK(default_blit_earth_)
	->Args({320, 240})
	->Args({7680, 4320});

BENCHMARK(blit_earth_without_alpha_)
	->Args({320, 240})
	->Args({7680, 4320});

BENCHMARK(blit_earth_with_memcpy_)
    ->Args({320, 240})
    ->Args({7680, 4320});

BENCHMARK_CAPTURE(blit_without_alpha_, "128x128 to 320x240", 128, 128)->Args({320, 240});
BENCHMARK_CAPTURE(blit_without_alpha_, "128x128 to 1280x720", 128, 128)->Args({1280, 720});
BENCHMARK_CAPTURE(blit_without_alpha_, "128x128 to 1920x1080", 128, 128)->Args({1920, 1080});
BENCHMARK_CAPTURE(blit_without_alpha_, "128x128 to 7680x4320", 128, 128)->Args({7680, 4320});

BENCHMARK_CAPTURE(blit_without_alpha_, "1024x1024 to 320x240", 1024, 1024)->Args({320, 240});
BENCHMARK_CAPTURE(blit_without_alpha_, "1024x1024 to 1280x720", 1024, 1024)->Args({1280, 720});
BENCHMARK_CAPTURE(blit_without_alpha_, "1024x1024 to 1920x1080", 1024, 1024)->Args({1920, 1080});
BENCHMARK_CAPTURE(blit_without_alpha_, "1024x1024 to 7680x4320", 1024, 1024)->Args({7680, 4320});

BENCHMARK_CAPTURE(blit_with_memcpy_, "128x128 to 320x240", 128, 128)->Args({320, 240});
BENCHMARK_CAPTURE(blit_with_memcpy_, "128x128 to 1280x720", 128, 128)->Args({1280, 720});
BENCHMARK_CAPTURE(blit_with_memcpy_, "128x128 to 1920x1080", 128, 128)->Args({1920, 1080});
BENCHMARK_CAPTURE(blit_with_memcpy_, "128x128 to 7680x4320", 128, 128)->Args({7680, 4320});

BENCHMARK_CAPTURE(blit_with_memcpy_, "1024x1024 to 320x240", 1024, 1024)->Args({320, 240});
BENCHMARK_CAPTURE(blit_with_memcpy_, "1024x1024 to 1280x720", 1024, 1024)->Args({1280, 720});
BENCHMARK_CAPTURE(blit_with_memcpy_, "1024x1024 to 1920x1080", 1024, 1024)->Args({1920, 1080});
BENCHMARK_CAPTURE(blit_with_memcpy_, "1024x1024 to 7680x4320", 1024, 1024)->Args({7680, 4320});

// BENCHMARK( my_other_blit_ )
//	->Args( { 320, 240 } )
//	->Args( { 7680, 4320 } )
//;

BENCHMARK_MAIN();
