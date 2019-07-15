#ifndef FARB_UI_NODE_H
#define FARB_UI_NODE_H

#include <string>
#include <memory>

#include "../../lib/tigr/tigr.h"

#include "../reflection/ReflectionDeclare.h"
#include "../core/BuiltinTypedefs.h"
#include "../core/NamedType.hpp"
#include "../core/ValueCheckedType.hpp"
#include "../utils/StringExtensions.hpp"
#include "Fonts.hpp"
#include "InputHandler.hpp"

namespace Farb
{

namespace UI
{

struct TigrDeleter
{
	void operator()(Tigr* bmp) { tigrFree(bmp); }
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

struct Image
{
	std::string filePath;

	// the same loaded file could be used by any number of Image objects
	// rather than loading copies
	// but needs to be deleted using tigrFree
	// which we pass in to the costructor of std::shared_ptr
	std::shared_ptr<Tigr> bitmap;

	Dimensions spriteLocation;

	Image()
		: filePath()
		, bitmap()
		, spriteLocation()
	{ }

	Image(Tigr* bitmap)
		: filePath()
		, bitmap(bitmap, tigrFree)
		, spriteLocation(0, 0, bitmap->w, bitmap->h)
	{ }

	static Reflection::TypeInfo* GetStaticTypeInfo();
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

enum class SizeType
{
	Scalar,
	FitContents,
	FitChildren
};

struct Size
{
	Scalar scalar;
	SizeType type;

	Size()
		: scalar()
		, type(SizeType::FitContents)
	{ }

	static Reflection::TypeInfo* GetStaticTypeInfo();
};

struct Node
{
	Input::Handler inputHandler;
	Image image;
	Text text;
	Scalar top, left, right, bottom;
	// rmf note: Nodes intentionally do not have a scale, just a size
	// there is no transform applied to children. Adding one might be a mistake
	Size width, height;
	std::vector<Node> children;

	static Reflection::TypeInfo* GetStaticTypeInfo();

	// rmf todo: postload verification that none of the children use PercentParent
	// if the SizeType is FitChildren
};

} // namespace UI


template <> Reflection::TypeInfo* Reflection::GetTypeInfo<UI::Units>();
template <> Reflection::TypeInfo* Reflection::GetTypeInfo<UI::SizeType>();

} // namespace Farb

#endif // FARB_UI_NODE_H