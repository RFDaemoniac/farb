#ifndef TEST_DESERIALIZE_HPP
#define TEST_DESERIALIZE_HPP

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

class TestDeserialize : public ITest
{
public:
	virtual bool RunTests() const override
	{
		{
			
		}

		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_DESERIALIZE_HPP
