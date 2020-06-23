#ifndef TEST_UI_TREE_HPP
#define TEST_UI_TREE_HPP

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDeclare.h"
#include "../../src/interface/UINode.h"
#include "../../src/interface/UIWindow.h"
#include "../../lib/tigr/tigr.h"
#include "../../src/serialization/Deserialization.h"

namespace Farb
{
using namespace Reflection;
using namespace UI;
using namespace Input;

namespace Tests
{

constexpr float windowDuration = 0.7f;

class TestUITree : public ITest
{
public:
	virtual bool RunTests() const override
	{
		std::cout << "Interface" << std::endl;

		Node root;
		auto rootReflect = Reflect(root);
		bool success = DeserializeFile("./tests/files/input/TestUITree.json", rootReflect);
		farb_print(success, "deserialize UI Tree");
		assert(success);

		UIWindow window{160, 90, "Test Window"};
		Node empty;
		float elapsedTime = 0.0f;
		while (elapsedTime < windowDuration)
		{
			success = window.Render(empty);
			elapsedTime += tigrTime();
			if (!success)
			{
				farb_print(success, "render empty Tree");
			}
			assert(success);
		}
		farb_print(success, "render empty Tree");

		elapsedTime = 0.0f;
        std::cout << ToString(root) << std::endl;
		while (elapsedTime < windowDuration)
		{
			success = window.Render(root);
			elapsedTime += tigrTime();
			if (!success)
			{
				farb_print(success, "render test Tree");
			}
			assert(success);
		}
		farb_print(success, "render test Tree");
		
		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_UI_TREE_HPP
