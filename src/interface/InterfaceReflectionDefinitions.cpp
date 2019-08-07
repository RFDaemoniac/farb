#include <iostream>
#include <set>
#include <unordered_map>

#include "UINode.h"
#include "Fonts.hpp"
#include "Window.h"
#include "InputHandler.hpp"
#include "TigrExtensions.h"
#include "ReflectionDeclare.h"
#include "ReflectionDefine.hpp"
#include "ReflectionBasics.h"
#include "ReflectionWrappers.hpp"
#include "ReflectionContainers.hpp"
#include "Deserialization.h"
#include "MapReduce.hpp"

namespace Farb
{
using namespace Reflection;

template<>
TypeInfo* Reflection::GetTypeInfo<TPixel>()
{
	// in TPixel the layout is b, g, r, a
	// but we are intentionally ordering reflection as rgba instead
	// because that is my preference
	static TypeInfoStruct<TPixel> typeInfo {
		"TPixel",
		nullptr,
		std::vector<MemberInfo<TPixel>*> {
			MakeMemberInfoTyped("r", &TPixel::r),
			MakeMemberInfoTyped("g", &TPixel::g),
			MakeMemberInfoTyped("b", &TPixel::b),
			MakeMemberInfoTyped("a", &TPixel::a)
		}
	};

	return &typeInfo;
}

TypeInfo* UI::Dimensions::GetStaticTypeInfo()
{
	static TypeInfoStruct<UI::Dimensions> typeInfo {
		"UI::Dimensions",
		nullptr,
		std::vector<MemberInfo<UI::Dimensions>*> {
			MakeMemberInfoTyped("x", &UI::Dimensions::x),
			MakeMemberInfoTyped("y", &UI::Dimensions::y),
			MakeMemberInfoTyped("width", &UI::Dimensions::width),
			MakeMemberInfoTyped("height", &UI::Dimensions::height)
		}
	};

	return &typeInfo;
}

struct NineSliceConverter : public Functor<ErrorOr<std::vector<UI::Dimensions> >, const std::vector<int> & >
{
	virtual ErrorOr<std::vector<UI::Dimensions> > operator()(const std::vector<int> & in) override
	{
		if (in.size() != 4)
		{
			return Error("NineSlice requires 4 coordinates, x1, x2, y1, y2. The rest x0, x3, y0, y3 are taken from dimensions.");
		}

		std::vector<UI::Dimensions> ret(9);
		using namespace UI::NineSliceLocations;

		// we can't set all the coordinates here, must wait until Image::PostLoad
		// for the ones derrived from spriteLocation
		ret[TC].x = in[0];
		ret[TR].x = in[1];
		ret[ML].y = in[2];
		ret[BL].y = in[3];
		return ret;
	}
};

TypeInfo* UI::Image::GetStaticTypeInfo()
{
	static NineSliceConverter converter;
	static auto nineSliceTypeInfo = TypeInfoAs<std::vector<UI::Dimensions>, std::vector<int> >(
		"NineSlice",
		converter);

	static TypeInfoStruct<UI::Image> typeInfo {
		"UI::Image(Table)",
		nullptr,
		std::vector<MemberInfo<UI::Image>*> {
			MakeMemberInfoTyped("source", &UI::Image::filePath),
			MakeMemberInfoTyped("tiled", &UI::Image::enableTiling),
			MakeMemberInfoTyped("dimensions", &UI::Image::spriteLocation),
			// would like to specify this as a vector of 4 ints [x1, x2, y1, y2]
			MakeMemberInfoTyped("slices", &UI::Image::nineSlice, &nineSliceTypeInfo)
		},
		&UI::Image::PostLoad
	};
	return &typeInfo;
}

bool UI::Image::PostLoad(Image& image)
{
	static std::unordered_map<std::string, std::weak_ptr<Tigr> > sharedImages;
	if (image.filePath.empty()) return false;

	if (sharedImages.count(image.filePath))
	{
		// rmf todo: what is the weak_ptr interface?
		// I should probably consider downloading the docs...
		if (auto tempShared = sharedImages[image.filePath].lock())
		{
			image.bitmap.reset(tempShared.get(), TigrDeleter());
		}
	}
	if (image.bitmap == nullptr)
	{
		image.bitmap.reset(tigrLoadImage(image.filePath.c_str()), TigrDeleter());
		sharedImages[image.filePath] = image.bitmap;
	}
	if (image.bitmap == nullptr) return false;

	if (image.spriteLocation.width == 0
		&& image.spriteLocation.height == 0)
	{
		image.spriteLocation.width = image.bitmap->w;
		image.spriteLocation.height = image.bitmap->h;
	}

	if (image.nineSlice.size() > 0)
	{
		using namespace UI::NineSliceLocations;
		int xCoordinates [4] { 
			image.spriteLocation.x,
			image.nineSlice[TC].x,
			image.nineSlice[TR].x,
			image.spriteLocation.x + image.spriteLocation.width
		};

		int yCoordinates [4] {
			image.spriteLocation.y,
			image.nineSlice[ML].y,
			image.nineSlice[BL].y,
			image.spriteLocation.y + image.spriteLocation.height
		};

		auto sliceFromCoords = [&](NineSliceLocations::Enum loc, int x, int y) {
			image.nineSlice[loc] = {
				xCoordinates[x],
				yCoordinates[y],
				xCoordinates[x+1] - xCoordinates[x],
				yCoordinates[y+1] - yCoordinates[y]
			};
		};

		sliceFromCoords(TL, 0, 0);
		sliceFromCoords(TC, 1, 0);
		sliceFromCoords(TR, 2, 0);

		sliceFromCoords(ML, 0, 1);
		sliceFromCoords(ML, 1, 1);
		sliceFromCoords(MR, 2, 1);

		sliceFromCoords(BL, 0, 2);
		sliceFromCoords(BC, 1, 2);
		sliceFromCoords(BR, 2, 2);
	}
	// height and width > 0 is to protect against dividing by 0
	// and feels nonsensical anyways...
	return image.bitmap != nullptr
		&& image.spriteLocation.height > 0
		&& image.spriteLocation.width > 0;
}

struct ScalarAssign
{
	static bool Parse(UI::Scalar& object, std::string value)
	{
		auto parts = Split(value, ' ');
		if (parts.size() != 2)
		{
			Error("Scalar::ParseAssign requires a single space between amount and units").Log();
			return false;
		}
		bool success = DeserializeString(parts[0], Reflect(object.amount));
		success = success && Reflect(object.units).AssignString(parts[1]);
		return success;
	}

	template<typename T>
	static bool Numeric(UI::Scalar& object, T value)
	{
		object.amount = static_cast<float>(value);
		object.units = UI::Units::Pixels;
		return true;
	}
};

TypeInfo* UI::Scalar::GetStaticTypeInfo()
{

	static auto typeInfo = TypeInfoCustomLeaf<UI::Scalar>::Construct(
		"UI::Scalar",
		ScalarAssign::Parse,
		ScalarAssign::Numeric<uint>,
		ScalarAssign::Numeric<int>,
		ScalarAssign::Numeric<float>
	);
	return &typeInfo;
}

TypeInfo* UI::Text::GetStaticTypeInfo()
{
	struct Assign
	{
		static bool String(UI::Text& object, std::string value)
		{
			// rmf todo: default text size and font?
			object.unparsedText = value;
			return true;
		}
	};

	// rmf todo: struct with assign functions
	static TypeInfoStruct<UI::Text> typeInfo {
		"UI::Text",
		nullptr,
		std::vector<MemberInfo<UI::Text>*> {
			MakeMemberInfoTyped("contents", &UI::Text::unparsedText),
			MakeMemberInfoTyped("size", &UI::Text::size),
			MakeMemberInfoTyped("font", &UI::Text::fontName)
		}
	};
	return &typeInfo;
}

struct UISizeAssign
{
	static bool Parse(UI::Size& object, std::string value)
	{
		bool valueWasEnum = Reflect(object.type).AssignString(value);
		if (valueWasEnum)
		{
			if (object.type == UI::SizeType::Scalar)
			{
				Error("Scalar Size values should be specified directly").Log();
				return false;
			}
			return true;
		}
		else
		{
			return Reflect(object.scalar).AssignString(value);
		}
	}

	template<typename T>
	static bool Numeric(UI::Size& object, T value)
	{
		object.scalar.amount = static_cast<float>(value);
		object.scalar.units = UI::Units::Pixels;
		return true;
	}
};

TypeInfo* UI::Size::GetStaticTypeInfo()
{
	static auto typeInfo = TypeInfoCustomLeaf<UI::Size>::Construct(
		"UI::Size",
		UISizeAssign::Parse,
		UISizeAssign::Numeric<uint>,
		UISizeAssign::Numeric<int>,
		UISizeAssign::Numeric<float>
	);
	return &typeInfo;
}

TypeInfo* UI::Node::GetStaticTypeInfo()
{
	static TypeInfoStruct<UI::Node> typeInfo {
		"UI::Node",
		nullptr,
		std::vector<MemberInfo<UI::Node>*> {
			MakeMemberInfoTyped("background", &UI::Node::backgroundColor),
			MakeMemberInfoTyped("image", &UI::Node::image),
			MakeMemberInfoTyped("text", &UI::Node::text),
			MakeMemberInfoTyped("input", &UI::Node::inputHandler),
			MakeMemberInfoTyped("top", &UI::Node::top),
			MakeMemberInfoTyped("left", &UI::Node::left),
			MakeMemberInfoTyped("right", &UI::Node::right),
			MakeMemberInfoTyped("bottom", &UI::Node::bottom),
			MakeMemberInfoTyped("height", &UI::Node::height),
			MakeMemberInfoTyped("width", &UI::Node::width),
			MakeMemberInfoTyped("children", &UI::Node::children)
		},
		UI::Node::PostLoad
	};
	return &typeInfo;
}

bool UI::Node::PostLoad(Node& node)
{
	NodeSpec& spec = node.spec;
	if (node.top.units != Units::None) spec |= NodeSpec::Top;
	if (node.left.units != Units::None) spec |= NodeSpec::Left;
	if (node.right.units != Units::None) spec |= NodeSpec::Right;
	if (node.bottom.units != Units::None) spec |= NodeSpec::Bottom;

	if (node.width.type != SizeType::Scalar
		|| node.width.scalar.units != Units::None)
	{
		spec |= NodeSpec::Width;
	}
	if (node.height.type != SizeType::Scalar
		|| node.height.scalar.units != Units::None)
	{
		spec |= NodeSpec::Height;
	}

	if (spec & NodeSpec::Top
		&& spec & NodeSpec::Bottom
		&& spec & NodeSpec::Height)
	{
		Error("UI::Node should specify at most two of top, bottom, and height").Log();
		return false;
	}
	if (spec & NodeSpec::Left
		&& spec & NodeSpec::Right
		&& spec & NodeSpec::Width)
	{
		Error("UI::Node should specify at most two of left, right, and width").Log();
		return false;
	}

	if (!node.image.filePath.empty()
		&& !node.text.unparsedText.empty())
	{
		// rmf note: I think this is necessary because we only compute 
		// a single dimension struct and use it to render contents
		Error("UI::Node should specify at most one of image and text").Log();
		return false;
	}
	if (node.image.filePath.empty()
		&& node.text.unparsedText.empty()
		&& (node.width.type == SizeType::FitContents
			|| node.height.type == SizeType::FitContents))
	{
		Error("UI::Node with width or height fit contents must have an image or text contents").Log();
		return false;
	}

	int depIndex = 0;
	std::set<DimensionAttribute> usedAttributes;

	auto TryAdd = [&](DimensionAttribute next)
	{
		if (usedAttributes.count(next))
		{
			return;
		}
		node.dependencyOrdering[depIndex++] = next;
		usedAttributes.insert(next);
	};

	if (spec & NodeSpec::Left)
	{
		TryAdd(DimensionAttribute::X);
		if (spec & NodeSpec::Right)
		{
			TryAdd(DimensionAttribute::Width);
		}
	}
	if (spec & NodeSpec::Top)
	{
		TryAdd(DimensionAttribute::Y);
		if (spec & NodeSpec::Bottom)
		{
			TryAdd(DimensionAttribute::Height);
		}
	}
	if (spec & NodeSpec::Width && node.width.type == SizeType::Scalar)
	{
		TryAdd(DimensionAttribute::Width);
	}
	if (spec & NodeSpec::Height && node.height.type == SizeType::Scalar)
	{
		TryAdd(DimensionAttribute::Height);
	}
	if (!node.image.filePath.empty())
	{
		if (spec & NodeSpec::Width && node.width.type == SizeType::FitContents)
		{
			TryAdd(DimensionAttribute::Width);
		}
		if (spec & NodeSpec::Height && node.height.type == SizeType::FitContents)
		{
			TryAdd(DimensionAttribute::Height);
		}
	}

	if (!usedAttributes.count(DimensionAttribute::Width)
		&& !usedAttributes.count(DimensionAttribute::Height))
	{
		Error("Could not parse dependencies for at least one of Width or Height").Log();
		return false;
	}

	if (!node.text.unparsedText.empty())
	{
		if (spec & NodeSpec::Width && node.width.type == SizeType::FitContents)
		{
			TryAdd(DimensionAttribute::Width);
		}
		else if (spec & NodeSpec::Height && node.height.type == SizeType::FitContents)
		{
			TryAdd(DimensionAttribute::Height);
		}
	}

	TryAdd(DimensionAttribute::Children);

	bool childrenBeforeWidth = !usedAttributes.count(DimensionAttribute::Width);
	bool childrenBeforeHeight = !usedAttributes.count(DimensionAttribute::Height);

	if (usedAttributes.size() < 5)
	{
		TryAdd(DimensionAttribute::Width);
		TryAdd(DimensionAttribute::X);
		TryAdd(DimensionAttribute::Height);
		TryAdd(DimensionAttribute::Y);
	}

	if (!childrenBeforeWidth && !childrenBeforeHeight)
	{
		return true;
	}
	for (const auto & child : node.children)
	{
		if (childrenBeforeWidth)
		{
			Scalar widthScalars[]{child.left, child.right, child.width.scalar};
			for (const auto & scalar : widthScalars)
			{
				if (scalar.units == Units::PercentOfParent)
				{
					Error("Scalar for child uses percent of parent width but parent width depends on children").Log();
					return false;
				}
			}
		}
		if (childrenBeforeHeight)
		{
			Scalar heightScalars[]{child.top, child.bottom, child.height.scalar};
			for (const auto & scalar : heightScalars)
			{
				if (scalar.units == Units::PercentOfParent)
				{
					Error("Scalar for child uses percent of parent height but parent height depends on children").Log();
					return false;
				}
			}
		}
	}
	return true;
}

// rmf todo: should probably do this as member deserialization not type deserialization

/*
template<>
TypeInfo* Reflection::GetTypeInfo<Input::Handler::Result(*)(Input::Event)>()
{
	struct Assign
	{
		static bool FunctionPointerFromString(Input::Handler::Result(*&object)(Input::Event) , std::string value)
		{
			Error("Input Handler response deserialization not implemented").Log();
			return true;
		}
	};

	static auto typeInfo = TypeInfoCustomLeaf<Input::Handler::Result(*)(Input::Event)>::Construct(
		"Input::Handler response",
		Assign::FunctionPointerFromString
	);
	return &typeInfo;
}
*/


TypeInfo* Input::Handler::GetStaticTypeInfo()
{
	static TypeInfoStruct<Input::Handler> typeInfo {
		"Input::Handler",
		nullptr,
		std::vector<MemberInfo<Input::Handler>*> {
			MakeMemberInfoTyped("type", &Input::Handler::inputType),
			MakeMemberInfoTyped("response", &Input::Handler::responseFunctionName)
		}
	};
	return &typeInfo;
}

template <>
TypeInfo* Reflection::GetTypeInfo<UI::Units>()
{
	static TypeInfoEnum<UI::Units> typeInfo {
		"UI::Units",
		std::vector<std::pair <std::string, int> >{
			// None is not reflected intentionally, because it should not be deserialized
			// could add for serialization if we need it
			// but would rather just not serialize none values at all
			{"px", static_cast<int>(UI::Units::Pixels)},
			{"%", static_cast<int>(UI::Units::PercentOfParent)},
			{"v", static_cast<int>(UI::Units::PercentOfScreen)}
		},
	};
	return &typeInfo;
}

template <>
TypeInfo* Reflection::GetTypeInfo<UI::SizeType>()
{
	static TypeInfoEnum<UI::SizeType> typeInfo {
		"UI::SizeType",
		std::vector<std::pair <std::string, int> > {
			// Scalar is not reflected intentionally because UI::Size doesn't need it
			// could add later
			{"FitContents", static_cast<int>(UI::SizeType::FitContents)},
			{"FitChildren", static_cast<int>(UI::SizeType::FitChildren)}
		},
	};
	return &typeInfo;
}

template <>
TypeInfo* Reflection::GetTypeInfo<Input::Type>()
{
	static TypeInfoEnum<Input::Type> typeInfo {
		"Input::Type",
		std::vector<std::pair <std::string, int> > {
			{"MouseDown", static_cast<int>(Input::Type::MouseDown)},
			{"MouseUp", static_cast<int>(Input::Type::MouseUp)},
			{"MouseClick", static_cast<int>(Input::Type::MouseClick)},
			{"KeyDown", static_cast<int>(Input::Type::KeyDown)},
			{"KeyUp", static_cast<int>(Input::Type::KeyUp)},
			{"KeyPress", static_cast<int>(Input::Type::KeyPress)}
		},
	};
	return &typeInfo;
}

} // namespace Farb