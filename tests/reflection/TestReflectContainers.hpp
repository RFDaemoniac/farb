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
		std::cout << "Test reflect std::vector<int>" << std::endl;
		std::vector<int> viTest;

		ReflectionObject viReflect = ReflectionObject::Construct(viTest);

		bool nameMatches = viReflect.typeInfo->GetName() == "std::vector<int>";
		farb_print(nameMatches, "std:vector<int> name is " + viReflect.typeInfo->GetName());
		assert(nameMatches);

		assert(viTest.size() == 0);
		bool success = viReflect.PushBackDefault();
		success = success && viTest.size() == 1 && viTest[0] == 0;
		farb_print(success, "reflect std::vector<int> PushBackDefault");
		assert(success);

		ReflectionObject iReflect = FARB_CHECK(
			viReflect.GetAtIndex(0),
			"reflect std::vector<int> GetAtIndex");
		FARB_ASSERT_ERROR(
			viReflect.GetAtIndex(1),
			"reflect std::vector<int> GetAtIndex invalid");
		success = iReflect.AssignInt(2);
		farb_print(success && viTest[0] == 2, "reflect std::vector<int> assign to element int");
		assert(success && viTest[0] == 2);


		std::cout << "Test reflect std::unordered_set<int>" << std::endl;

		std::unordered_set<int> siTest;
		ReflectionObject setReflect = ReflectionObject::Construct(siTest);
		assert(siTest.size() == 0);

		success = setReflect.PushBackDefault();
		iReflect = FARB_CHECK(
			setReflect.GetAtIndex(0),
			"reflect std::unordered_set<int> GetAtIndex");
		success = iReflect.AssignInt(2);
		int * pAssignedValue = reinterpret_cast<int*>(iReflect.location);
		farb_print(success && *pAssignedValue == 2, "reflect std::unordered_set<int> buffered assign");
		assert(success && *pAssignedValue == 2);
		success = setReflect.ArrayEnd();
		farb_print(success, "reflect std::unordered_set<int> array end");
		farb_print(siTest.size() == 1, "reflect std::unordered_set<int> buffered insert correct length");
		farb_print(siTest.count(2), "reflect std::unordered_set<int> buffered insert correct value");
		assert(success && siTest.size() == 1 && siTest.count(2));

		return true;
	}
};

}

} // namespace Farb

#endif // TEST_REFLECTION_CONTAINERS_H
