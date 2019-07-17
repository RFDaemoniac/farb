#ifndef FARB_TIGR_EXTENSIONS_H
#define FARB_TIGR_EXTENSIONS_H

#include "../../lib/tigr/tigr.h"
#include "../core/ErrorOr.hpp"

namespace Farb
{

namespace UI
{

struct TigrDeleter
{
	void operator()(Tigr* bmp) { if (bmp != nullptr) tigrFree(bmp); }
};

struct Dimensions
{
	int x, y;
	int width, height;

	Dimensions()
		: x(0), y(0), width(0), height(0)
	{ }

	Dimensions(int x, int y, int width, int height)
		: x(x), y(y), width(width), height(height)
	{ }
};

enum class DimensionAttribute
{
	X,
	Y,
	Width,
	Height,
	Children
};

struct Image
{
	std::string filePath;

	// the same loaded file could be used by any number of Image objects
	// rather than loading copies
	// but needs to be deleted using tigrFree
	// which we pass in to the costructor of std::shared_ptr
	std::shared_ptr<Tigr> bitmap;

	Dimensions spriteLocation;

	std::vector<Dimensions> nineSlice;

	Image()
		: filePath()
		, bitmap()
		, spriteLocation()
	{ }

	Image(Tigr* bitmap)
		: filePath()
		, bitmap(bitmap, TigrDeleter())
		, spriteLocation(0, 0, bitmap->w, bitmap->h)
	{ }

	static Reflection::TypeInfo* GetStaticTypeInfo();
};

namespace NineSliceLocations
{
	enum Enum
	{
		UL, UC, UR,
		ML, MC, MR,
		BL, BC, BR,
	}
}

inline ErrorOr<Success> TigrDrawImage(
	Tigr* destImage,
	const Dimensions& destDim,
	const Image& source)
{
	using NineSliceLocations;
	if (destDim.width == source.spriteLocation.width
		&& destDim.height == source.spriteLocation.height)
	{
		tigrBlit(destImage, source.bitmap.get(),
			destDim.x, destDim.y,
			source.spriteLocation.x, source.spriteLocation.y
			source.spriteLocation.width, source.spriteLocation.height);
		return Success();
	}
	else if (source.nineSlice.size() == 9)
	{
		int spriteX = source.spriteLocation.x;
		int spriteY = source.spriteLocation.y;
		auto sourceImage = source.bitmap.get();
		auto& slices = source.nineSlice;
		int middleWidth = destDim.width - slices[UL].width - slices[UR].width;
		int middleHeight = destDim.height - slices[UL].height - slices[BL].height;
		if (middleWidth < 0|| middleHeight < 0)
		{
			return Error("Truncated 9 sliced images are not implemented");
		}
		auto blitCorner = [&](NineSliceLocations::Enum corner)
		{
			auto& slice = source.nineSlice[corner];
			tigrBlit(destImage, sourceImage
				destDim.x + slice.x - spriteX,
				destDim.y + slice.y - spriteY,
				slice.x,
				slice.y,
				slice.width,
				slice.height);
		}
		const NineSliceLocations::Enum corners[] {UL, UR, BL, BR};
		for (auto corner : corners)
		{
			blitCorner(corner);
		}
		int remainingHeight = middleHeight;
		while (remainingHeight > 0)
		{
			int remainingWidth = middleWidth;
			while (remainingWidth > 0)
			{
				
			}
		}
	}
	else if (destDim.width <= source.spriteLocation.width
		&& destDim.height <= source.spriteLocation.height)
	{
		return Error("Truncated images are not implemented");
	}
	// rmf note: probably won't do scaled images, we could just render the image here anyways
	// at normal scale
	return Error("Scaled images (without 9 slicing) not implemented");
}

} // namespace UI


} // namespace Farb

#endif // FARB_TIGR_EXTENSIONS_H