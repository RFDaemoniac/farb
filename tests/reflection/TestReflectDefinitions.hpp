#ifndef TEST_REFLECTION_DEFINITIONS_H
#define TEST_REFLECTION_DEFINITIONS_H

#include "../../src/reflection/ReflectionDeclare.h"
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

static TypeInfoEnum<ExampleEnum> exampleEnumTypeInfo {
	"ExampleEnum",
	std::vector<std::pair <std::string, int> >{
		{"NegativeTwo", -2},
		{"Zero", 0},
		{"One", 1},
		{"Two", 2}
	},
};

} // namespace Tests

using namespace Tests;

template <>
TypeInfo* Reflection::GetTypeInfo<ExampleEnum>()
{
	return &exampleEnumTypeInfo;
}


namespace Tests
{

struct ExampleBaseStruct
{
	// rmf todo: additional member types
	ExampleEnum e1;
	ExampleEnum e2;

	static TypeInfoStruct<ExampleBaseStruct> typeInfo;
	virtual TypeInfo* GetTypeInfo() const { return &typeInfo; }
};

TypeInfoStruct<ExampleBaseStruct> ExampleBaseStruct::typeInfo = TypeInfoStruct<ExampleBaseStruct> {
	"ExampleBaseStruct",
	nullptr,
	std::vector<MemberInfo<ExampleBaseStruct>*> {
		new MemberInfoTyped<ExampleBaseStruct, decltype(ExampleBaseStruct::e1)> {"e1", &ExampleBaseStruct::e1},
		new MemberInfoTyped<ExampleBaseStruct, decltype(ExampleBaseStruct::e1)> {"e2", &ExampleBaseStruct::e2}
	},
};
/*
struct ExampleDerivedStruct : public ExampleBaseStruct
{
	ExampleEnum e3;
	ExampleEnum e4;

	static TypeInfoStruct<ExampleDerivedStruct> typeInfo;
	virtual TypeInfo* GetTypeInfo() const override { return typeInfo; }
};

ExampleDerivedStruct::typeInfo {
	"ExampleDerivedStruct",
	GetTypeInfo<ExampleBaseStruct>(),
	std::vector<std::shared_ptr<TypeInfoStruct<ExampleDerivedStruct>::MemberInfo> > {
		new TypeInfoStruct<ExampleDerivedStruct>::MemberInfoTyped {"e3", &ExampleDerivedStruct::e3, Reflection::GetTypeInfo<decltype(ExampleDerivedStruct::e3)>()},
		new TypeInfoStruct<ExampleDerivedStruct>::MemberInfoTyped {"e4", &ExampleDerivedStruct::e4, Reflection::GetTypeInfo<decltype(ExampleDerivedStruct::e4)>()}
	}
};
*/
} // namespace Tests

} // namespace Farb

#endif // TEST_REFLECTION_DEFINITIONS_H