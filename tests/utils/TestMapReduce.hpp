#ifndef TEST_MAP_REDUCE_HPP
#define TEST_MAP_REDUCE_HPP

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/utils/MapReduce.hpp"

namespace Farb
{

namespace Tests
{

bool to_bool(int i)
{
	return i > 0;
}

int to_int(bool b)
{
	if (b) return 1;
	return 0;
}


bool foo(char c, int i, bool b)
{
	return c > 0 && i > 0 && b;
}

int bar(float f)
{
	if (f > 0.0f)
	{
		return 1;
	}
	return -1;
}

class TestMapReduce : public ITest
{
public:
	virtual bool RunTests() const override
	{
		std::cout << "MapReduce" << std::endl;

		{
			auto b = FunctionPointer(to_bool);
			auto i = FunctionPointer(to_int);

			auto composed = ComposedFunctors<bool, TypeList<>, int, TypeList<>, TypeList<bool>>(b, i);

			auto result = composed(true);
			farb_print(result, "composed functions with 1 argument each");
			assert(result);
		}

		{
			auto f = FunctionPointer(foo);
			auto b = FunctionPointer(bar);

			auto composed = Compose(f, b);

			auto result = composed('a', 0.2f, true);

			farb_print(result, "composed functions with multiple arguments");
			assert(result);

		}

		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_MAP_REDUCE_HPP
