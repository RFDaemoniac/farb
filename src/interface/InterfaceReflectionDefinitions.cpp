#include <iostream>

#include "../../lib/tigr/tigr.h"

#include "UINode.h"
#include "Fonts.hpp"
#include "InputHandler.hpp"
#include "../reflection/ReflectionDeclare.h"
#include "../reflection/ReflectionDefine.hpp"
#include "../reflection/ReflectionBasics.h"
#include "../reflection/ReflectionWrappers.hpp"
#include "../reflection/ReflectionContainers.hpp"
#include "../serialization/Deserialization.h"

namespace Farb
{
using namespace Reflection;

TypeInfo* UI::Image::GetStaticTypeInfo()
{
	struct Assign
	{
		static bool String(UI::Image& object, std::string value)
		{
			object.filePath = value;
			object.bitmap.reset(tigrLoadImage(object.filePath.c_str()), tigrFree);
			if (object.bitmap != nullptr)
			{
				object.spriteLocation.width = object.bitmap->w;
				object.spriteLocation.height = object.bitmap->h;
			}
			return object.bitmap != nullptr;
		}
	};

	static auto typeInfo = TypeInfoCustomLeaf<UI::Image>::Construct(
		"UI::Image",
		Assign::String
	);
	return &typeInfo;
}

struct UIScalarAssign
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
		UIScalarAssign::Parse,
		UIScalarAssign::Numeric<uint>,
		UIScalarAssign::Numeric<int>,
		UIScalarAssign::Numeric<float>
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
		}
	};
	return &typeInfo;
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
		std::vector<std::pair <std::string, int> >{
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
		std::vector<std::pair <std::string, int> >{
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