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
		assert(s.i2 == 2);

		ReflectionObject member = FARB_CHECK(
			sReflect.GetAtKey("e1"),
			"reflect struct get member enum");
		bool success = member.AssignString("Zero");

		farb_print(success && s.e1 == ExampleEnum::Zero, "reflect struct assign to member enum");
		assert(success && s.e1 == ExampleEnum::Zero);

		member = FARB_CHECK(
			sReflect.GetAtKey("i2"),
			"reflect struct get member int");

		success = member.AssignInt(0);
		farb_print(success && s.i2 == 0, "reflect struct assign to member int");
		assert(success && s.i2 == 0);

		FARB_ASSERT_ERROR(
			sReflect.GetAtKey("nonexistent member"),
			"reflect struct get nonexistent member");

		return true;
	}
};

}

} // namespace Farb

#endif // TEST_REFLECTION_STRUCT_H
