#include "TigrExtensions.h"


namespace Farb
{

namespace UI
{

Dimensions Image::GetDestinationForSlice(
	const Dimensions& destTotal,
	NineSliceLocations::Enum location) const
{
	Dimensions dest = destTotal;
	using NineSliceLocations;
	const NineSliceLocations::Enum corners[] {UL, UR, BL, BR};
	const auto& slice = nineSlice[location];

	if (IsLeft(location))
	{
		dest.width = slice.width;
	}
	else if (IsRight(location))
	{
		dest.x += destTotal.width - slice.width;
		dest.width = slice.width;
	}
	else
	{
		dest.x += nineSlice[UL].width;
		dest.width -= (nineSlice[UL].width + nineSlice[UR].width);
	}

	if (IsUpper(location))
	{
		dest.height = slice.height;
	}
	else if (IsBottom(location))
	{
		dest.y += destTotal.height - slice.height;
		dest.height = slice.height;
	}
	else
	{
		dest.y += nineSlice[UL].height;
		dest.height -= (nineSlice[UL].height + nineSlice[BL].height);
	}

	return dest;
}


ErrorOr<Success> TigrBlitWrapped(
	Tigr* destImage,
	const Dimensions& destTotal,
	Tigr* sourceImage,
	const Dimensions& sourceDim)
{
	if ((sourceDim.width == 0 || sourceDim.height == 0)
		&& destTotal.width != 0
		&& destTotal.height != 0)
	{
		return Error("Trying to BlitWrapped a section with no size to a destination with size");
	}
	int destX = destTotal.x;
	int destY = destTotal.y;
	int xRepeat = destDim.width / sourceDim.width;
	int xRemainder = destDim.width % sourceDim.width;
	int yRepeat = destDim.height / sourceDim.height;
	int yRemainder = destDim.height % sourceDim.height;
	auto blit = [&](int destX, int destY, int width, int height)
	{
		tigrBlit(
			destImage, sourceImage,
			destX, destY,
			sourceDim.x, sourceDim.y,
			width, height);
	}

	for (int row = 0; row < yRepeat; ++row)
	{
		int destY = destTotal.y + (row * sourceDim.height);
		for (int col = 0; col < xRepeat; ++col)
		{
			blit(
				destTotal.x + (col * sourceDim.width),
				destY
				sourceDim.width,
				sourceDim.height);
		}
	}

	if (yRemainder > 0)
	{
		destY = destTotal.y + (yRepeat * sourceDim.height);
		for (int col = 0; col < xRepeat; ++col)
		{
			blit(
				destTotal.x + (col * sourceDim.width),
				destY,
				sourceDim.width,
				yRemainder);
		}
	}

	if (xRemainder > 0)
	{
		destX = destTotal.x + (xRepeat * sourceDim.width);
		for (int row = 0; row < yRepeat; ++row)
		{
			blit(
				destX,
				destTotal.y + (row * sourceDim.height),
				xRemainder,
				sourceDim.height);
		}
	}

	if (yRemainder > 0 && xRemainder > 0)
	{
		blit(
			destTotal.x + (xRepeat * sourceDim.width),
			destTotal.y + (yRepeat * sourceDim.height),
			xRemainder,
			yRemainder);
	}
	return Success();
}

ErrorOr<Success> Image::Draw(
	Tigr* destImage,
	const Dimensions& destDim,
	const Image& source)
{
	using NineSliceLocations;
	if (destDim.width == source.spriteLocation.width
		&& destDim.height == source.spriteLocation.height)
	{
		// simplest case is shortcuted, this might not be worth it if it's not common
		tigrBlit(destImage, source.bitmap.get(),
			destDim.x, destDim.y,
			source.spriteLocation.x, source.spriteLocation.y
			source.spriteLocation.width, source.spriteLocation.height);
		return Success();
	}
	else if (source.nineSlice.size() == 9)
	{
		if (destDim.width <= source.nineSlice[UL].width + source.nineSlice[UR].width
			&& destDim.height <= source.nineSlice[UL].height + source.nineSlice[BL].height)
		{
			return Error("9 sliced images with truncated corners are not implemented");
		}
		const NineSliceLocations::Enum corners[] {UL, UR, BL, BR};
		for (auto location : corners)
		{
			Dimensions destSlicedDim = source.GetDestinationForSlice(destDim, location);
			tigrBlit(destImage, sourceImage,
				destSlicedDim.x, destSlicedDim.y,
				source.nineSlice[location].width, source.nineSlice[location].height);
		}
		const NineSliceLocations::Enum cross[] {UC, ML, MC, MR, BC};
		for (auto location : cross)
		{
			Dimensions destSlicedDim = source.GetDestinationForSlice(destDim, location);
			CHECK_RETURN(TigrBlitWrapped(destImage, destSlicedDim,
				sourceImage, source.nineSlice[location]));
		}
		return Success();
	}
	else if (source.enableTiling)
	{
		// wrapped will handle repeated images
		CHECK_RETURN(TigrBlitWrapped(destImage, destDim, sourceImage, source.spriteLocation));
		return Success();
	}
	else if (destDim.width <= source.spriteLocation.width
		&& destDim.height <= source.spriteLocation.height)
	{
		// wrapped will handle truncated images as well as repeated images
		TigrBlitWrapped(destImage, destDim, sourceImage, source.spriteLocation);
		return Success();
	}
	
	// rmf note: probably won't do scaled images,
	// we could just render the image here at normal scale?
	return Error("Scaled images (without 9 slicing, or tiling) not implemented");
}

} // namespace UI

} // namespace Farb
