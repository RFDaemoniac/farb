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

bool duplicated(bool b, int i1, int i2, int i3)
{
	return b && i1 > 0 && i2 > 0 && i3 > 0;
}

int shared(char c)
{
	if (c > 0) return 1;
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

		{
			auto d = FunctionPointer(duplicated);

			auto reduced = RemoveDuplicateParam<int>(d);

			auto result = reduced(true, 1, 2);

			farb_print(result, "copying value for duplicate param");
			assert(result);
		}

		{
			auto f = FunctionPointer(foo);
			auto s = FunctionPointer(shared);

			// we don't need to specify the shared param type here
			// because it's assumed that it's the first argument for each
			auto composed = ComposeWithSharedParam(f, s);

			auto result = composed('a', true);

			farb_print(result, "composing with duplicated param");
			assert(result);
		}

		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_MAP_REDUCE_HPP
