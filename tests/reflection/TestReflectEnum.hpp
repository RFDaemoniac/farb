#ifndef TEST_REFLECTION_ENUM_H
#define TEST_REFLECTION_ENUM_H

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDefine.hpp"


namespace Farb
{

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

} // namespace Tests


template <>
Reflection::TypeInfo* Reflection::GetTypeInfo<Tests::ExampleEnum>(const Tests::ExampleEnum& obj)
{
	return &Tests::exampleEnumTypeInfo;
}

namespace Tests
{

class TestReflectEnum : public ITest
{
public:
	virtual bool RunTests() const override
	{
		ExampleEnum e = ExampleEnum::NegativeTwo;
		Reflection::TypeInfo* typeInfo = Reflection::GetTypeInfo<ExampleEnum>(e);
		assert(e == ExampleEnum::NegativeTwo);
		bool success = typeInfo->AssignString(reinterpret_cast<byte*>(&e), "One");
		farb_print(success && e == ExampleEnum::One, "reflect enum assign string");
		assert(success && e == ExampleEnum::One);
		success = typeInfo->AssignString(reinterpret_cast<byte*>(&e), "Invalid Value");
		farb_print(!success && e == ExampleEnum::One, "reflect enum assign invalid string");
		assert(!success && e == ExampleEnum::One);
		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_REFLECTION_ENUM_H
