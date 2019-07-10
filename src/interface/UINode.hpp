#ifndef FARB_UI_NODE_HPP
#define FARB_UI_NODE_HPP

#include <string>

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

struct Image
{
	std::string filePath;
	// this pointer is owned by image
	// but needs to be deleted using tigrFree
	Tigr* bitmap;

	~Image()
	{
		if (bitmap != nullptr)
		{
			// commented out because linking tigr is something I want to do later
			// tigrFree(bitmap);
		}
	}

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
	Size height, width;
	std::vector<Node> children;

	static Reflection::TypeInfo* GetStaticTypeInfo();
};

} // namespace UI


template <> Reflection::TypeInfo* Reflection::GetTypeInfo<UI::Units>();
template <> Reflection::TypeInfo* Reflection::GetTypeInfo<UI::SizeType>();

} // namespace Farb

#endif // FARB_UI_NODE_HPP