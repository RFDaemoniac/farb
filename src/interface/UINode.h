#ifndef FARB_UI_NODE_H
#define FARB_UI_NODE_H

#include <string>
#include <memory>

#include "../reflection/ReflectionDeclare.h"
#include "../core/BuiltinTypedefs.h"
#include "../core/NamedType.hpp"
#include "../core/ValueCheckedType.hpp"
#include "../utils/StringExtensions.hpp"
#include "TigrExtensions.h"
#include "Fonts.hpp"
#include "InputHandler.hpp"

namespace Farb
{

namespace UI
{

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
		, type(SizeType::Scalar)
	{ }

	static Reflection::TypeInfo* GetStaticTypeInfo();
};

enum class NodeSpec
{
	None   = 1 << 0,
	Top    = 1 << 1,
	Left   = 1 << 2,
	Right  = 1 << 3,
	Bottom = 1 << 4,
	Width  = 1 << 5,
	Height = 1 << 6
};

inline NodeSpec operator|(NodeSpec a, NodeSpec b)
{
	return static_cast<NodeSpec>(static_cast<int>(a) | static_cast<int>(b));
}

inline NodeSpec& operator|=(NodeSpec& a, const NodeSpec& b)
{
	a = a | b;
	return a;
}

inline bool operator&(NodeSpec a, NodeSpec b)
{
	return static_cast<bool>(static_cast<int>(a) & static_cast<int>(b));
}

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

	// these are not reflected and are runtime only
	NodeSpec spec = NodeSpec::None;
	DimensionAttribute dependencyOrdering[5];

	static Reflection::TypeInfo* GetStaticTypeInfo();

	static bool PostLoad(Node& node);

	// rmf todo: postload verification that none of the children use PercentParent
	// if the SizeType is FitChildren
};

} // namespace UI


template <> Reflection::TypeInfo* Reflection::GetTypeInfo<UI::Units>();
template <> Reflection::TypeInfo* Reflection::GetTypeInfo<UI::SizeType>();

} // namespace Farb

#endif // FARB_UI_NODE_H