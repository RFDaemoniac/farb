#include <algorithm>

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
	const Dimensions & slice = nineSlice[location];
	using namespace NineSliceLocations;

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
		dest.x += nineSlice[TL].width;
		dest.width -= (nineSlice[TL].width + nineSlice[TR].width);
	}

	if (IsTop(location))
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
		dest.y += nineSlice[TL].height;
		dest.height -= (nineSlice[TL].height + nineSlice[BL].height);
	}

	// apply cropping
	dest.width = std::max(0, std::min(dest.width, destTotal.x - dest.x + destTotal.width));
	dest.height = std::max(0, std::min(dest.height, destTotal.y - dest.y + destTotal.height));

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
	int xRepeat = destTotal.width / sourceDim.width;
	int xRemainder = destTotal.width % sourceDim.width;
	int yRepeat = destTotal.height / sourceDim.height;
	int yRemainder = destTotal.height % sourceDim.height;
	auto blit = [&](int destX, int destY, int width, int height)
	{
		tigrBlit(
			destImage, sourceImage,
			destX, destY,
			sourceDim.x, sourceDim.y,
			width, height);
	};

	for (int row = 0; row < yRepeat; ++row)
	{
		int destY = destTotal.y + (row * sourceDim.height);
		for (int col = 0; col < xRepeat; ++col)
		{
			blit(
				destTotal.x + (col * sourceDim.width),
				destY,
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
	const Dimensions& destDim) const
{
	auto & source = (*this);
	using namespace NineSliceLocations;
	if (destDim.width == source.spriteLocation.width
		&& destDim.height == source.spriteLocation.height)
	{
		// simplest case is shortcuted, this might not be worth it if it's not common
		tigrBlit(destImage, source.bitmap.get(),
			destDim.x, destDim.y,
			source.spriteLocation.x, source.spriteLocation.y,
			source.spriteLocation.width, source.spriteLocation.height);
		return Success();
	}
	else if (source.nineSlice.size() == 9)
	{
		if (destDim.width <= source.nineSlice[TL].width + source.nineSlice[TR].width
			&& destDim.height <= source.nineSlice[TL].height + source.nineSlice[BL].height)
		{
			return Error("9 sliced images with truncated corners are not implemented");
		}
		const NineSliceLocations::Enum corners[] {TL, TR, BL, BR};
		for (auto location : corners)
		{
			Dimensions destSlicedDim = source.GetDestinationForSlice(destDim, location);
			if (destSlicedDim.width == 0 || destSlicedDim.height == 0)
			{
				continue;
			}
			tigrBlit(destImage, source.bitmap.get(),
				destSlicedDim.x, destSlicedDim.y,
				source.nineSlice[location].x, source.nineSlice[location].y,
				destSlicedDim.width, destSlicedDim.height);
		}
		const NineSliceLocations::Enum cross[] {TC, ML, MC, MR, BC};
		for (auto location : cross)
		{
			Dimensions destSlicedDim = source.GetDestinationForSlice(destDim, location);
			if (destSlicedDim.width == 0 || destSlicedDim.height == 0)
			{
				continue;
			}
			CHECK_RETURN(TigrBlitWrapped(destImage, destSlicedDim,
				source.bitmap.get(), source.nineSlice[location]));
		}
		return Success();
	}
	else if (source.enableTiling)
	{
		// wrapped will handle repeated images
		CHECK_RETURN(TigrBlitWrapped(destImage, destDim, source.bitmap.get(), source.spriteLocation));
		return Success();
	}
	else if (destDim.width <= source.spriteLocation.width
		&& destDim.height <= source.spriteLocation.height)
	{
		// wrapped will handle truncated images
		CHECK_RETURN(TigrBlitWrapped(destImage, destDim, source.bitmap.get(), source.spriteLocation));
		return Success();
	}
	
	// rmf note: probably won't do scaled images,
	// we could just render the image here at normal scale?
	return Error("Scaled images (without 9 slicing, or tiling) not implemented");
}

ErrorOr<Success> Text::UpdateParsedText()
{
	// rmf todo actual replacement and localization
	cachedParsedText = unparsedText;
	return Success();
}

struct DrawGlyphFunctor
{
	const Dimensions& destDim;
	TigrFont* font;

	DrawGlyphFunctor(const Dimensions& destDim, TigrFont* font)
		: destDim(destDim)
		, font(font)
	{ }

	void operator()(TigrGlyph* g, int x, int y)
	{
		x = x + destDim.x;
		y = y + destDim.y;
		// glyph can be partially clipped if we're over the destDim height
		// (or width, less likely due to wrapping)
		w = std::min(g->w, destDim.width - x);
		h = std::min(g->h, destDim.height - y);
		if (w < 0 || h < 0) return;
		tigrBlitTint(destImage, font->bitmap, x, y, g->x, g->y, w, h, color);
	}
}

// rmf todo: wrap by word, not character
// including skipping spaces at the start of a line
std::pair<int, int> Text::WalkthroughBounds(
	int maxWidth,
	DrawGlyphFunctor* drawFunctor) const
{
	const char * p = cachedParsedText.c_str();
	int line = 0;
	int lineWidth = 0;
	int maxLineWidth = 0;
	TigrFont * font = GetFont(fontName);
	TigrGlyph * g;
	int c;
	int lineHeight = tigrTextHeight(font, "");

	bool skipNextSpaceAtStartOfLine = false;

	while (*p)
	{
		p = tigrDecodeUTF8(p, &c);
		if (c == '\r') continue;
		if (c == '\n')
		{
			if (lineWidth > maxLineWidth)
			{
				maxLineWidth = lineWidth;
			}
			// this is not wraparound, spaces at the start of the line
			// are considered indentation
			skipNextSpaceAtStartOfLine = false;
			line++;
			lineWidth = 0;
			continue;
		}
		g = get(font, c);
		int potentialLineWidth = lineWidth + g->w;
		if (widthBound >= 0 && potentialLineWidth > widthBound)
		{
			if (potentialLineWidth > maxLineWidth)
			{
				maxLineWidth = widthBound;
			}
			// this is wraparound, don't need to include spaces
			skipNextSpaceAtStartOfLine = true;
			line++;
			lineWidth = 0;
		}
		if (skipNextSpaceAtStartOfLine
			&& lineWidth == 0
			&& c == ' ')
		{
			// should we skip multiple spaces in a row?
			skipNextSpaceAtStartOfLine = false;
			continue;
		}
		if (drawFunctor != nullptr)
		{
			(*drawFunctor)(g, lineWidth, line * lineHeight);
		}
		lineWidth += g->w;
	}
	return { maxLineWidth, (line + 1) * lineHeight };
}

std::pair<int, int> Text::GetBoundsRequired(int maxWidth) const
{
	return WalkthroughBounds(maxWidth, nullptr);
}

ErrorOr<Success> Text::Draw(
	Tigr* destImage,
	const Dimensions& destDim) const
{
	// when would we return error?
	// if text is clipped?

	DrawGlyphFunctor draw { destDim, GetFont(fontName) };
	WalkthroughBounds(destDim.width, draw);

	return Success();
}



} // namespace UI

} // namespace Farb
