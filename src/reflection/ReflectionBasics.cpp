#include <limits.h>

#include "ReflectionDeclare.h"
#include "ReflectionDefine.hpp"

namespace Farb
{

namespace Reflection
{

template <>
TypeInfo* GetTypeInfo<bool>()
{
	static auto boolTypeInfo = TypeInfoCustomLeaf<bool>::Construct(
		"bool",
		static_cast<bool (*)(bool&, bool)>([](bool& object, bool value)
		{
			object = value;
			return true;
		}),
		HString("uint"), static_cast<bool (*)(bool&, uint)>([](bool& object, uint value)
		{
			switch(value)
			{
			case 0:
				object = false;
				return true;
			case 1:
				object = true;
				return true;
			default:
				return false;
			}
		}),
		HString("int"), static_cast<bool (*)(bool&, int)>([](bool& object, int value)
		{
			switch(value)
			{
			case 0:
				object = false;
				return true;
			case 1:
				object = true;
				return true;
			default:
				return false;
			}
		})
	);
	return &boolTypeInfo;
}

template <>
TypeInfo* GetTypeInfo<char>()
{
	static auto charTypeInfo = TypeInfoCustomLeaf<char>::Construct(
		"char",
		HString("uint"), static_cast<bool (*)(char&, uint)>([](char& object, uint value)
		{
			object = static_cast<char>(value);
			return true;
		}),
		HString("int"), static_cast<bool (*)(char&, int)>([](char& object, int value)
		{
			object = static_cast<char>(value);
			return true;
		})
	);
	return &charTypeInfo;
}


template <>
TypeInfo* GetTypeInfo<unsigned char>()
{
	static auto ucharTypeInfo = TypeInfoCustomLeaf<unsigned char>::Construct(
		"char",
		HString("uint"), static_cast<bool (*)(unsigned char&, uint)>([](unsigned char& object, uint value)
		{
			object = static_cast<unsigned char>(value);
			return true;
		}),
		HString("int"), static_cast<bool (*)(unsigned char&, int)>([](unsigned char& object, int value)
		{
			if (value < 0) return false;
			object = static_cast<unsigned char>(value);
			return true;
		})
	);
	return &ucharTypeInfo;
}


template <>
TypeInfo* GetTypeInfo<uint>()
{
	static auto uintTypeInfo = TypeInfoCustomLeaf<uint>::Construct(
		"uint",
		static_cast<bool (*)(uint&, uint)>([](uint& object, uint value)
		{
			object = value;
			return true;
		}),
		HString("int"), static_cast<bool (*)(uint&, int)>([](uint& object, int value)
		{
			if (value < 0 ) { return false; }
			object = static_cast<uint>(value);
			return true;
		})
	);
	return &uintTypeInfo;
}

template <>
TypeInfo* GetTypeInfo<int>()
{
	static auto intTypeInfo = TypeInfoCustomLeaf<int>::Construct(
		"int",
		static_cast<bool (*)(int&, uint)>([](int& object, uint value)
		{
			if (value > INT_MAX) { return false; }
			object = static_cast<int>(value);
			return true;
		}),
		static_cast<bool (*)(int&, int)>([](int& object, int value)
		{
			object = value;
			return true;
		})
	);
	return &intTypeInfo;
}

template <>
TypeInfo* GetTypeInfo<float>()
{
	static auto floatTypeInfo = TypeInfoCustomLeaf<float>::Construct(
		"float",
		static_cast<bool (*)(float&, uint)>([](float& object, uint value)
		{
			object = static_cast<float>(value);
			return true;
		}),
		static_cast<bool (*)(float&, int)>([](float& object, int value)
		{
			object = static_cast<float>(value);
			return true;
		}),
		static_cast<bool (*)(float&, float)>([](float& object, float value)
		{
			object = value;
			return true;
		})
	);

	return &floatTypeInfo;
}

template <>
TypeInfo* GetTypeInfo<std::string>()
{
	static auto stringTypeInfo = TypeInfoCustomLeaf<std::string>::Construct(
		"std::string",
		static_cast<bool (*)(std::string&, std::string)>([](std::string& object, std::string value)
		{
			object = value;
			return true;
		})
	);
	return &stringTypeInfo;
}

} // namespace Reflection

} // namespace Farb
