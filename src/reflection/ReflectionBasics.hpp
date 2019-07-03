#ifndef FARB_REFLECTION_BASICS_H
#define FARB_REFLECTION_BASICS_H

#include <limits.h>

#include "ReflectionDeclare.h"
#include "ReflectionDefine.hpp"

namespace Farb
{

namespace Reflection
{

static auto boolTypeInfo = TypeInfoCustomLeaf<bool>::Construct(
	"bool",
	static_cast<bool (*)(bool&, bool)>([](bool& object, bool value)
	{
		object = value;
		return true;
	}),
	static_cast<bool (*)(bool&, uint)>([](bool& object, uint value)
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
	static_cast<bool (*)(bool&, int)>([](bool& object, int value)
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

template <>
TypeInfo* GetTypeInfo<bool>()
{
		return &boolTypeInfo;
}

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

template <>
TypeInfo* GetTypeInfo<int>()
{
	return &intTypeInfo;
}

static auto uintTypeInfo = TypeInfoCustomLeaf<uint>::Construct(
	"uint",
	static_cast<bool (*)(uint&, uint)>([](uint& object, uint value)
	{
		object = value;
		return true;
	}),
	static_cast<bool (*)(uint&, int)>([](uint& object, int value)
	{
		if (value < 0 ) { return false; }
		object = static_cast<uint>(value);
		return true;
	})
);

template <>
TypeInfo* GetTypeInfo<uint>()
{
	return &uintTypeInfo;
}

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

template <>
TypeInfo* GetTypeInfo<float>()
{
	return &floatTypeInfo;
}

static auto stringTypeInfo = TypeInfoCustomLeaf<std::string>::Construct(
	"std::string",
	static_cast<bool (*)(std::string&, std::string)>([](std::string& object, std::string value)
	{
		object = value;
		return true;
	})
);

template <>
TypeInfo* GetTypeInfo<std::string>()
{
	return &stringTypeInfo;
}

} // namespace Reflection

} // namespace Farb


#endif // FARB_REFLECTION_BASICS_H
