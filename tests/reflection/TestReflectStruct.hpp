#ifndef TEST_REFLECTION_STRUCT_H
#define TEST_REFLECTION_STRUCT_H

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDefine.hpp"
#include "TestReflectDefinitions.hpp"


namespace Farb
{

namespace Tests
{
using namespace Reflection;


class TestReflectStruct : public ITest
{
public:
	virtual bool RunTests() const override
	{
		ExampleBaseStruct s;
		ReflectionObject sReflect = ReflectionObject::Construct(s);
		assert(s.e1 == ExampleEnum::One);
		assert(s.e2 == ExampleEnum::Two);

		ReflectionObject member = FARB_CHECK(
			sReflect.GetAtKey("e1"),
			"reflect struct get member");
		bool success = member.AssignString("Zero");

		farb_print(success && s.e1 == ExampleEnum::Zero, "reflect struct assign to member enum");
		assert(success && s.e1 == ExampleEnum::Zero);

		FARB_ASSERT_ERROR(
			sReflect.GetAtKey("nonexistent member"),
			"reflect struct get nonexistent member");

		return true;
	}
};

}

} // namespace Farb

#endif // TEST_REFLECTION_STRUCT_H
