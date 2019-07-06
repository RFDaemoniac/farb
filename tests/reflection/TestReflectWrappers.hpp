#ifndef TEST_REFLECTION_WRAPPERS_HPP
#define TEST_REFLECTION_WRAPPERS_HPP

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDeclare.h"
#include "../../src/reflection/ReflectionBasics.hpp"
#include "../../src/reflection/ReflectionWrappers.hpp"
#include "TestReflectDefinitions.hpp"

namespace Farb
{
using namespace Reflection;

namespace Tests
{

class TestReflectWrappers : public ITest
{
public:
	virtual bool RunTests() const override
	{
		{
			ExampleNamedTypeInt namedTest;
			ReflectionObject namedReflect = Reflect(namedTest);
			PrintTestName(namedReflect);
			assert(namedTest.value == 0);
			bool success = namedReflect.AssignInt(1);
			farb_print(success && namedTest.value == 1, "reflect named type int assign valid type");
			assert(success && namedTest.value == 1);
			success = namedReflect.AssignString("invalid");
			farb_print(!success && namedTest.value == 1, "reflect named type int assign invalid type");
			assert(!success && namedTest.value == 1);
		}
		{
			ExampleValueCheckedTypeEvenInt checkedTest;
			auto checkedReflect = Reflect(checkedTest);
			PrintTestName(checkedReflect);
			assert(checkedTest.GetValue() == 0);
			
			bool success = checkedReflect.AssignInt(2);
			farb_print(success && checkedTest.GetValue() == 2, "reflect checked type int assign valid value");
			assert(success && checkedTest.GetValue() == 2);
			success = checkedReflect.AssignInt(1);
			farb_print(!success && checkedTest.GetValue() == 2, "reflect checked type int assign invalid value");
			assert(!success && checkedTest.GetValue() == 2);
			success = checkedReflect.AssignString("invalid");
			farb_print(!success && checkedTest.GetValue() == 2, "reflect checked type int assign invalid type");
			assert(!success && checkedTest.GetValue() == 2);
		}

		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_REFLECTION_WRAPPERS_HPP
