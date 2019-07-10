
#include "UINode.hpp"
#include "Fonts.hpp"
#include "InputHandler.hpp"
#include "../reflection/ReflectionDefine.hpp"

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
			// rmf todo: value check type for whether it exists?
			// could also do loading here, too
			return true;
		}
	};

	static TypeInfoCustomLeaf<UI::Image> typeInfo {
		"UI::Image",
		Assign::String
	}
	return &typeInfo;
}

TypeInfo* UI::Scalar::GetStaticTypeInfo()
{
	struct Assign
	{
		static bool Parse(UI::Scalar& object, std::string value)
		{
			auto parts = Split(value, ' ');
			if (parts.size() != 2)
			{
				Error("Scalar::ParseAssign requires a single space between amount and units").Log();
				return false;
			}
			bool success = Reflect(object.amount).AssignString(parts[0]);
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

	static TypeInfoCustomLeaf<UI::Scalar> typeInfo {
		"UI::Scalar",
		Assign::Parse,
		Assign::Numeric<uint>,
		Assign::Numeric<int>,
		Assign::Numeric<float>
	}
	return &typeInfo;
}

TypeInfo* UI::Text::GetStaticTypeInfo()
{
	struct Assign
	{
		bool String(UI::Text& object, std::string value)
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
	}
	return &typeInfo;
}

TypeInfo* UI::Size::GetStaticTypeInfo()
{
	struct Assign
	{
		bool Parse(UI::Size& object, std::string value)
		{
			bool valueWasEnum = Reflect(object.type).AssignString(value);
			if (valueWasEnum)
			{
				if (type == SizeType::Scalar)
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

	static TypeInfoCustomLeaf<UI::Size> typeInfo {
		"UI::Size",
		Assign::Parse,
		Assign::Numeric<uint>,
		Assign::Numeric<int>,
		Assign::Numeric<float>
	}
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
	}
	return &typeInfo;
}

// rmf todo: should probably do this as member deserialization not type deserialization
template<>
TypeInfo* GetTypeInfo<Input::Handler::Result(*)(Event)>()
{
	struct Assign
	{
		bool FunctionPointerFromString(UI::Size& object, std::string value)
		{
			Error("Input Handler response deserialization not implemented").Log();
			return true;
		}
	};

	static TypeInfoCustomLeaf<Input::Handler::Result(*)(Event)> typeInfo {
		"Input::Handler response",
		Assign::FunctionPointerFromString,
	}
	return &typeInfo;
}

TypeInfo* Input::Handler::GetStaticTypeInfo()
{
	static TypeInfoStruct<Input::Handler> typeInfo {
		"Input::Handler",
		nullptr,
		std::vector<MemberInfo<Input::Handler>*> {
			MakeMemberInfoTyped("type", &Input::Handler::inputType),
			MakeMemberInfoTyped("response", &Input::Handler::response)
		}
	}
	return &typeInfo;
}

template <>
TypeInfo* GetTypeInfo<UI::Units>()
{
	static TypeInfoEnum<UI::Units> typeInfo {
		"UI::Units",
		std::vector<std::pair <std::string, int> >{
			// None is not reflected intentionally, because it should not be deserialized
			// could add for serialization if we need it
			// but would rather just not serialize none values at all
			{"px", UI::Units::Pixels},
			{"%", UI::Units::PercentOfParent},
			{"v", UI::Units::PercentOfScreen}
		},
	};
	return &typeInfo;
}

template <>
TypeInfo* GetTypeInfo<UI::SizeType>()
{
	static TypeInfoEnum<UI::SizeType> typeInfo {
		"UI::SizeType",
		std::vector<std::pair <std::string, int> >{
			// Scalar is not reflected intentionally because UI::Size doesn't need it
			// could add later
			{"FitContents", UI::SizeType::FitContents},
			{"FitChildren", UI::SizeType::FitChildren}
		},
	};
	return &typeInfo;
}

template <>
TypeInfo* GetTypeInfo<Input::Type>()
{
	static TypeInfoEnum<Input::Type> typeInfo {
		"Input::Type",
		std::vector<std::pair <std::string, int> >{
			{"MouseDown", Input::Type::MouseDown},
			{"MouseUp", Input::Type::MouseUp},
			{"MouseClick", Input::Type::MouseClick},
			{"KeyDown", Input::Type::KeyDown},
			{"KeyUp", Input::Type::KeyUp},
			{"KeyPress", Input::Type::KeyPress}
		},
	};
	return &typeInfo;
}

} // namespace Farb