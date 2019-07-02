#ifndef TEST_REFLECTION_CONTAINERS_H
#define TEST_REFLECTION_CONTAINERS_H

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDefine.hpp"
#include "TestReflectDefinitions.hpp"


namespace Farb
{

namespace Tests
{
using namespace Reflection;


class TestReflectContainers : public ITest
{
public:
	virtual bool RunTests() const override
	{
		std::vector<int> viTest;

		ReflectionObject viReflect = ReflectionObject::Construct(viTest);

		assert(viTest.size() == 0);
		bool success = viReflect.PushBackDefault();
		farb_print(
			success
			&& viTest.size() == 1
			&& viTest[0] == 0,
			"reflect std::vector<int> PushBackDefault");
		assert(success && viTest.size() == 1 && viTest[0] == 0);

		ReflectionObject iReflect = FARB_CHECK(
			viReflect.GetAtIndex(0),
			"reflect std::vector<int> GetAtIndex");
		FARB_ASSERT_ERROR(
			viReflect.GetAtIndex(1),
			"reflect std::vector<int> GetAtIndex invalid");
		success = iReflect.AssignInt(2);
		farb_print(success && viTest[0] == 2, "reflect std::vector<int> assign to element int");
		assert(success && viTest[0] == 2);

		return true;
	}
};

}

} // namespace Farb

#endif // TEST_REFLECTION_CONTAINERS_H
