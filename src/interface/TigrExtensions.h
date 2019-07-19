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

	Dimensions(const Dimensions& other)
		: x(other.x), y(other.y), width(other.width), height(other.height)
	{ }

	void CopyPos(const Dimensions& other)
	{
		x = other.x;
		y = other.y;
	}

	void CopySize(const Dimensions& other)
	{
		width = other.width;
		height = other.height;
	}
};

enum class DimensionAttribute
{
	X,
	Y,
	Width,
	Height,
	Children
};

namespace NineSliceLocations
{
	enum Enum
	{
		UL, UC, UR, // 0, 1, 2
		ML, MC, MR, // 3, 4, 5
		BL, BC, BR, // 6, 7, 8
	}

	bool IsUpper(Enum e) { return e <= UR; }

	bool IsBottom(Enum e) { return e >= BL; }

	bool IsLeft(Enum e) { return e == UL || e == ML || e == BL; }

	bool IsRight(Enum e) { return e == UR || e == MR || e == BR; }
}

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

	Dimensions GetDestinationForSlice(
		const Dimensions& destTotal,
		NineSliceLocations::Enum location) const;

	static Reflection::TypeInfo* GetStaticTypeInfo();
};

void TigrBlitWrapped(
	Tigr* destImage,
	const Dimensions& destTotal,
	Tigr* sourceImage,
	const Dimensions& sourceDim);

ErrorOr<Success> TigrDrawImage(
	Tigr* destImage,
	const Dimensions& destDim,
	const Image& source);

} // namespace UI


} // namespace Farb

#endif // FARB_TIGR_EXTENSIONS_H