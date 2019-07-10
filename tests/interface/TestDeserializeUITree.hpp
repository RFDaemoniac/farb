#ifndef TEST_DESERIALIZE_UI_TREE_HPP
#define TEST_DESERIALIZE_UI_TREE_HPP

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDeclare.h"
#include "../../src/interface/UINode.hpp"
#include "../../src/serialization/Deserialization.h"

namespace Farb
{
using namespace Reflection;
using namespace UI;
using namespace Input;

namespace Tests
{


class TestDeserializeUITree : public ITest
{
public:
	virtual bool RunTests() const override
	{
		std::cout << "Interface" << std::endl;

		Node root;
		bool success = DeserializeFile("./tests/files/TestUITree.json", Reflect(root));

		farb_print(success, "deserialize UI Tree");
		assert(success);
		
		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_DESERIALIZE_UI_TREE_HPP
