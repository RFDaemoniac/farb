#ifndef FARB_TIGR_EXTENSIONS_H
#define FARB_TIGR_EXTENSIONS_H

#include "../../lib/tigr/tigr.h"

#include "../reflection/ReflectionDeclare.h"
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

	static Reflection::TypeInfo* GetStaticTypeInfo();
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

namespace NineSliceLocations
{
	enum Enum
	{
		TL, TC, TR, // 0, 1, 2
		ML, MC, MR, // 3, 4, 5
		BL, BC, BR, // 6, 7, 8
	};

	inline bool IsTop(Enum e) { return e <= TR; }

	inline bool IsBottom(Enum e) { return e >= BL; }

	inline bool IsLeft(Enum e) { return e == TL || e == ML || e == BL; }

	inline bool IsRight(Enum e) { return e == TR || e == MR || e == BR; }
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
		, nineSlice()
		, enableTiling(false)
	{ }

	Image(Tigr* bitmap)
		: filePath()
		, bitmap(bitmap, TigrDeleter())
		, spriteLocation(0, 0, bitmap->w, bitmap->h)
		, nineSlice()
		, enableTiling(false)
	{ }

	inline bool Defined() const { return !filePath.empty(); }

	Dimensions GetDestinationForSlice(
		const Dimensions& destTotal,
		NineSliceLocations::Enum location) const;

	ErrorOr<Success> Draw(
		Tigr* destImage,
		const Dimensions& destDim) const;

	static Reflection::TypeInfo* GetStaticTypeInfo();

	static bool PostLoad(Image& image);
};

/*
// rmf note: to be used if we're trying to minimize the width
// while expanding within a height requirement
// this is probably not necessary for a long time, if ever

struct Word
{
	bool punctuation;
	bool space;
	int index;
	int length;
}

struct Line
{
	// if the first word is a space, it should not contribute to width
	// this means width needs some upkeep, which is what the functions are for
	std::vector<Word> words;
	int width;

	// returns new width
	int RemoveLastWord();
	
	// returns new width
	int AddWordToFront(Word&& w);

	int AddWordToBack(Word&& w);
}

struct Paragraph
{
	std::vector<Line> lines;
	int widthMax;
	int widthCurrent;
	int lineHeight;
}
*/

struct DrawGlyphFunctor;

struct Text
{
	std::string unparsedText;
	std::string cachedParsedText;
	Scalar size;
	FontName fontName;
	TPixel color;

	inline bool Defined() const { return !unparsedText.empty(); }

	// rmf todo get context to pass in and use for replacements
	ErrorOr<Success> UpdateParsedText();

	std::pair<int, int> GetBoundsRequired(int maxWidth) const;

	ErrorOr<Success> Draw(
		Tigr* destImage,
		const Dimensions& destDim) const;

	static Reflection::TypeInfo* GetStaticTypeInfo();

private:
	// shared behavior between GetBoundsRequired and Draw
	std::pair<int, int> WalkthroughBounds(
		int maxWidth,
		DrawGlyphFunctor* drawFunctor) const;

	static TigrFont* GetFont(FontName name);
};

} // namespace UI


} // namespace Farb

#endif // FARB_TIGR_EXTENSIONS_H