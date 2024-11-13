#include "image.hpp"

#include <memory>
#include <algorithm>

#include <cstdio>
#include <cstring>
#include <cassert>

#include <stb_image.h>

#include "surface.hpp"

#include "../support/error.hpp"

namespace
{
	struct STBImageRGBA_ : public ImageRGBA
	{
		STBImageRGBA_( Index, Index, std::uint8_t* );
		virtual ~STBImageRGBA_();
	};
}

ImageRGBA::ImageRGBA()
	: mWidth( 0 )
	, mHeight( 0 )
	, mData( nullptr )
{}

ImageRGBA::~ImageRGBA() = default;


std::unique_ptr<ImageRGBA> load_image( char const* aPath )
{
	assert( aPath );

	stbi_set_flip_vertically_on_load( true );

	int w, h, channels;
	stbi_uc* ptr = stbi_load( aPath, &w, &h, &channels, 4 );
	if( !ptr )
		throw Error( "Unable to load image \"%s\"", aPath );

	return std::make_unique<STBImageRGBA_>(
		ImageRGBA::Index(w),
		ImageRGBA::Index(h),
		ptr
	);
}

void blit_masked(Surface& aSurface, ImageRGBA const& aImage, Vec2f aPosition)
{
    int startX = static_cast<int>(aPosition.x);
    int startY = static_cast<int>(aPosition.y);

    int imageWidth = aImage.get_width();
    int imageHeight = aImage.get_height();

    int surfaceWidth = aSurface.get_width();
    int surfaceHeight = aSurface.get_height();

    // Go through every pixel of the image
    for (int y = 0; y < imageHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {
            // Gets the current pixel color in the image
            ColorU8_sRGB_Alpha color = aImage.get_pixel(x, y);

            // Skip pixels with an Alpha value less than 128
            if (color.a < 128) {
                continue;
            }

            // Calculate the pixel position on the target Surface
            int destX = startX + x;
            int destY = startY + y;

            // Check that the target location is within range of the Surface
            if (destX >= 0 && destX < surfaceWidth && destY >= 0 && destY < surfaceHeight) {
                // Set the pixel color on the target Surface, ignoring the Alpha channel
                aSurface.set_pixel_srgb(destX, destY, {color.r, color.g, color.b});
            }
        }
    }
}

namespace
{
	STBImageRGBA_::STBImageRGBA_( Index aWidth, Index aHeight, std::uint8_t* aPtr )
	{
		mWidth = aWidth;
		mHeight = aHeight;
		mData = aPtr;
	}

	STBImageRGBA_::~STBImageRGBA_()
	{
		if( mData )
			stbi_image_free( mData );
	}
}
