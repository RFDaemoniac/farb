#ifndef FARB_UI_NODE_H
#define FARB_UI_NODE_H

#include <string>

#include "../reflection/ReflectionDeclare.h"
#include "../core/BuiltinTypedefs.h"
#include "../core/NamedType.hpp"
#include "../core/ValueCheckedType.hpp"

namespace Farb
{

namespace UI
{

struct FontNameTag
{
	static HString GetName() { return "FontName"; }
}

using FontName = NamedType<std::string, FontNameTag>;

struct Image
{
	std::string filePath;
	// rmf todo: loaded value as ?, check tigr
}

enum class Units
{
	None,
	Pixels,
	PercentOfParentMin, // could have width, height, min, max
	PercentOfScreenMin
}

struct Scalar
{
	float amount;
	Units units;

	Scalar()
		: amount(0.0)
		, units(Units::None)
	{ }

	bool ParseAssign(const std::string& value);
}

struct Text
{
	std::string unparsedText;
	std::string cachedParsedText;
	Scalar size;
	FontName fontName;
}

enum class SizeType
{
	Scalar,
	FitContents,
	FitChildren
}

struct Size
{
	Scalar scalar;
	SizeType type;

	Size()
		: scalar()
		, SizeType(SizeType::FitContents)
	{ }

	bool ParseAssign(const std::string& value);
}

struct Node
{
	std::string text;
	Scalar top, left, right, bottom;
	// rmf note: Nodes intentionally do not have a scale, just a size
	// there is no transform applied to children. Adding one might be a mistake
	Size height, width;
	std::vector<Node> children;
}

} // namespace UI

} // namespace Farb

#endif // FARB_UI_NODE_H