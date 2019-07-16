#ifndef TEST_UI_TREE_HPP
#define TEST_UI_TREE_HPP

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDeclare.h"
#include "../../src/interface/UINode.h"
#include "../../src/interface/Window.h"
#include "../../lib/tigr/tigr.h"
#include "../../src/serialization/Deserialization.h"

namespace Farb
{
using namespace Reflection;
using namespace UI;
using namespace Input;

namespace Tests
{


class TestUITree : public ITest
{
public:
	virtual bool RunTests() const override
	{
		std::cout << "Interface" << std::endl;

		Node root;
		bool success = DeserializeFile("./tests/files/TestUITree.json", Reflect(root));
		farb_print(success, "deserialize UI Tree");
		assert(success);

		Window window{160, 90, "Test Window"};
		Node empty;
		float elapsedTime = 0.0f;
		while (elapsedTime < 1.0f)
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
		
		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_UI_TREE_HPP
