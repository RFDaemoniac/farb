#ifndef FARB_TIGR_EXTENSIONS_H
#define FARB_TIGR_EXTENSIONS_H

#include "../../lib/tigr/tigr.h"
#include "../core/ErrorOr.hpp"

#include "Fonts.hpp"

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
};

enum class DimensionAttribute
{
	X,
	Y,
	Width,
	Height,
	Children
};

enum class Units
{
	None,
	Pixels,
	// could add width, height, min, max
	// these work based on dimension being specified
	PercentOfParent,
	PercentOfScreen
};

struct Scalar
{
	float amount;
	Units units;

	Scalar()
		: amount(0.0)
		, units(Units::None)
	{ }

	static Reflection::TypeInfo* GetStaticTypeInfo();
};

struct Text
{
	std::string unparsedText;
	std::string cachedParsedText;
	Scalar size;
	FontName fontName;

	static Reflection::TypeInfo* GetStaticTypeInfo();
};

namespace NineSliceLocations
{
	enum Enum
	{
		UL, UC, UR, // 0, 1, 2
		ML, MC, MR, // 3, 4, 5
		BL, BC, BR, // 6, 7, 8
	}

	inline bool IsUpper(Enum e) { return e <= UR; }

	inline bool IsBottom(Enum e) { return e >= BL; }

	inline bool IsLeft(Enum e) { return e == UL || e == ML || e == BL; }

	inline bool IsRight(Enum e) { return e == UR || e == MR || e == BR; }
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

	bool enableTiling;

	Image()
		: filePath()
		, bitmap()
		, spriteLocation()
		, enableTiling(false)
	{ }

	Image(Tigr* bitmap)
		: filePath()
		, bitmap(bitmap, TigrDeleter())
		, spriteLocation(0, 0, bitmap->w, bitmap->h)
		, enableTiling(false)
	{ }

	inline bool Defined() { return !filePath.empty(); }

	Dimensions GetDestinationForSlice(
		const Dimensions& destTotal,
		NineSliceLocations::Enum location) const;

	ErrorOr<Success> Draw(
		Tigr* destImage,
		const Dimensions& destDim) const;


	static Reflection::TypeInfo* GetStaticTypeInfo();
};

} // namespace UI


} // namespace Farb

#endif // FARB_TIGR_EXTENSIONS_H