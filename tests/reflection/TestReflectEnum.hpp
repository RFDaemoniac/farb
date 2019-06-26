#ifndef TEST_REFLECTION_ENUM_H
#define TEST_REFLECTION_ENUM_H

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDefine.hpp"
#include "TestReflectDefinitions.hpp"


namespace Farb
{

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
