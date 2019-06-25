#ifndef TEST_REFLECTION_ENUM_H
#define TEST_REFLECTION_ENUM_H

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionTypes.h"


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

class TestReflectEnum : public ITest
{
public:
	virtual bool RunTests() const override
	{
		// rmf note: need to find a place to declare/access this
		// maybe all of them can be accessed from a templated global function
		// need this for all basic types and classes I didn't define
		// inside classes I do define are useful for heirarchies
		// I don't think I need heirarchies for classes I don't define... but we'll see
		Reflection::TypeInfo* exEnumTypeInfo = new Reflection::TypeInfoEnum<ExampleEnum>{
			"ExampleEnum",
			nullptr,
			std::vector<std::pair <std::string, int> >{
				{"NegativeTwo", -2},
				{"Zero", 0},
				{"One", 1},
				{"Two", 2}
			},
		};
		ExampleEnum e = ExampleEnum::NegativeTwo;
		assert(e == ExampleEnum::NegativeTwo);
		bool success = exEnumTypeInfo->AssignString(reinterpret_cast<byte*>(&e), "One");
		assert_print(success && e == ExampleEnum::One, "reflect enum assign string");
		success = exEnumTypeInfo->AssignString(reinterpret_cast<byte*>(&e), "Invalid Value");
		assert(!success && e == ExampleEnum::One, "reflect enum assign invalid string");
		delete exEnumTypeInfo;
		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_REFLECTION_ENUM_H
