#ifndef TEST_REFLECTION_DEFINITIONS_H
#define TEST_REFLECTION_DEFINITIONS_H

#include "../../src/reflection/ReflectionDefine.hpp"


namespace Farb
{

using namespace Reflection;

namespace Tests
{

enum class ExampleEnum
{
	NegativeTwo = -2,
	Zero = 0,
	One = 1,
	Two = 2
};

static Reflection::TypeInfoEnum<ExampleEnum> exampleEnumTypeInfo {
	"ExampleEnum",
	nullptr,
	std::vector<std::pair <std::string, int> >{
		{"NegativeTwo", -2},
		{"Zero", 0},
		{"One", 1},
		{"Two", 2}
	},
};

struct ExampleBaseStruct
{
	// rmf todo: additional member types
	ExampleEnum e1;
	ExampleEnum e2;

	static TypeInfoStruct<ExampleBaseStruct> typeInfo;
	virtual TypeInfo* GetTypeInfo() const { return &typeInfo; }
};

ExampleBaseStruct::typeInfo = TypeInfoStruct {
	"ExampleBaseStruct",
	nullptr,
	std::vector<TypeInfoStruct::MemberInfo> >{
		{"e1", &ExampleBaseStruct::e1, GetTypeInfo<decltype(ExampleBaseStruct::e1)>()},
		{"e2", &ExampleBaseStruct::e2, GetTypeInfo<decltype(ExampleBaseStruct::e2)>()}
	},
};

struct ExampleDerivedStruct : public ExampleBaseStruct
{
	ExampleEnum e3;
	ExampleEnum e4;

	static TypeInfoStruct<ExampleDerivedStruct> typeInfo;
	virtual TypeInfo* GetTypeInfo() const override { return typeInfo; }
};

ExampleDerivedStruct::typeInfo {
	"ExampleDerivedStruct",
	nullptr,
	std::vector<TypeInfoStruct<ExampleDerivedStruct>::MemberInfo> > {
		{"e3", &ExampleDerivedStruct::e3, GetTypeInfo<decltype(ExampleBaseStruct::e3)>()},
		{"e4", &ExampleDerivedStruct::e4, GetTypeInfo<decltype(ExampleBaseStruct::e4)>()}
	}
};

} // namespace Tests

using namespace Tests;

template <>
TypeInfo* GetTypeInfo<ExampleEnum>(const ExampleEnum& obj)
{
	return &exampleEnumTypeInfo;
}

template <>
TypeInfo* GetTypeInfo<ExampleEnum>()
{
	return &exampleEnumTypeInfo;
}

} // namespace Farb

#endif // TEST_REFLECTION_DEFINITIONS_H