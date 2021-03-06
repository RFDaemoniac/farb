#ifndef TEST_REFLECTION_DEFINITIONS_H
#define TEST_REFLECTION_DEFINITIONS_H

#include "../../src/reflection/ReflectionDeclare.h"
#include "../../src/reflection/ReflectionDefine.hpp"
#include "../../src/reflection/ReflectionBasics.h"
#include "../../src/reflection/ReflectionContainers.hpp"
#include "../../src/core/NamedType.hpp"
#include "../../src/core/ValueCheckedType.hpp"


namespace Farb
{

using namespace Reflection;

namespace Tests
{

void PrintTestName(ReflectionObject ro)
{
	std::cout << "Reflect " << ro.typeInfo->GetName() << std::endl;
}

struct ExampleNamedTypeIntTag
{
	static HString GetName() { return "ExampleNamedTypeInt"; }
};

using ExampleNamedTypeInt = NamedType<int, ExampleNamedTypeIntTag>;

struct ExampleValueCheckedTypeEvenIntTag
{
	static HString GetName() { return "ExampleValueCheckedTypeEvenInt"; }

	static bool IsValid(const int& i) { return i % 2 == 0; }
};

using ExampleValueCheckedTypeEvenInt = ValueCheckedType<int, ExampleValueCheckedTypeEvenIntTag>;

enum class ExampleEnum
{
	NegativeTwo = -2,
	Zero = 0,
	One = 1,
	Two = 2
};

} // namespace Tests

using namespace Tests;

template <>
TypeInfo* Reflection::GetTypeInfo<ExampleEnum>()
{
	static TypeInfoEnum<ExampleEnum> exampleEnumTypeInfo {
		"ExampleEnum",
		std::vector<std::pair <std::string, int> >{
			{"NegativeTwo", -2},
			{"Zero", 0},
			{"One", 1},
			{"Two", 2}
		},
	};
	return &exampleEnumTypeInfo;
}


namespace Tests
{

struct ExampleBaseStruct
{
	// rmf todo: additional member types
	ExampleEnum e1;
	int i2;

	ExampleBaseStruct()
		: e1(ExampleEnum::One)
		, i2(2)
	{ }

	ExampleBaseStruct(ExampleEnum e1, int i2)
		: e1(e1)
		, i2(i2)
	{ }

	ExampleBaseStruct(const ExampleBaseStruct& other)
		: e1(other.e1)
		, i2(other.i2)
	{ }

	virtual ~ExampleBaseStruct()
	{ }

	bool operator ==(const ExampleBaseStruct& other)
	{
		return e1 == other.e1 && i2 == other.i2;
	}

	static TypeInfo* GetStaticTypeInfo()
	{
		static auto typeInfo = TypeInfoStruct<ExampleBaseStruct> {
			"ExampleBaseStruct",
			nullptr,
			std::vector<MemberInfo<ExampleBaseStruct>*> {
				new MemberInfoTyped<ExampleBaseStruct, decltype(ExampleBaseStruct::e1)> {"e1", &ExampleBaseStruct::e1},
				new MemberInfoTyped<ExampleBaseStruct, decltype(ExampleBaseStruct::i2)> {"i2", &ExampleBaseStruct::i2}
			},
		};
		return &typeInfo;
	}	
	virtual TypeInfo* GetInstanceTypeInfo() const { return GetStaticTypeInfo(); }
};

struct ExampleDerivedStruct : public ExampleBaseStruct
{
	ExampleEnum e3;
	int i4;

	static TypeInfo* GetStaticTypeInfo()
	{
		static auto typeInfo = TypeInfoStruct<ExampleDerivedStruct> {
			"ExampleDerivedStruct",
			Reflection::GetTypeInfo<ExampleBaseStruct>(),
			std::vector<MemberInfo<ExampleDerivedStruct>*> {
				new MemberInfoTyped<ExampleDerivedStruct, decltype(ExampleDerivedStruct::e3)> {"e3", &ExampleDerivedStruct::e3},
				new MemberInfoTyped<ExampleDerivedStruct, decltype(ExampleDerivedStruct::i4)> {"i4", &ExampleDerivedStruct::i4}
			}
		};
		return &typeInfo;
	}

	virtual TypeInfo* GetInstanceTypeInfo() const override { return GetStaticTypeInfo(); }
};

} // namespace Tests

} // namespace Farb

#endif // TEST_REFLECTION_DEFINITIONS_H