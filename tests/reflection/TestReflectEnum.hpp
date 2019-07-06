#ifndef TEST_REFLECTION_ENUM_H
#define TEST_REFLECTION_ENUM_H

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDeclare.h"
#include "TestReflectDefinitions.hpp"


namespace Farb
{
using namespace Reflection;

namespace Tests
{

class TestReflectEnum : public ITest
{
public:
	virtual bool RunTests() const override
	{
		ExampleEnum e = ExampleEnum::NegativeTwo;
		ReflectionObject eReflect = ReflectionObject::Construct(e);
		PrintTestName(eReflect);
		assert(e == ExampleEnum::NegativeTwo);
		bool success = eReflect.AssignString("One");
		farb_print(success && e == ExampleEnum::One, "reflect enum assign string");
		assert(success && e == ExampleEnum::One);
		success = eReflect.AssignString("Invalid Value");
		farb_print(!success && e == ExampleEnum::One, "reflect enum assign invalid string");
		assert(!success && e == ExampleEnum::One);
		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_REFLECTION_ENUM_H
