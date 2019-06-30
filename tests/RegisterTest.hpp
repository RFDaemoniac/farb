#ifndef REGISTER_TEST_H
#define REGISTER_TEST_H

#include <assert.h>
#include <iostream>
#include <string>

#include "../src/errors/ErrorOr.hpp"

namespace Farb
{


namespace Tests
{

#define FARB_CHECK(result, sTestName) \
	({ \
		auto r = result; \
		auto s = sTestName; \
		farb_print(r, s); \
		assert(!r.IsError()); \
		r.GetValue(); \
	})

#define FARB_ASSERT_ERROR(result, sTestName) \
	({ \
		auto r = result; \
		auto s = sTestName; \
		farb_print(r.IsError(), s); \
		assert(r.IsError()); \
		r.GetError(); \
	})

template<typename T>
static inline void farb_print (ErrorOr<T>& result, std::string sTestName)
{
	if (result.IsError())
	{
		std::cout << "## FAIL ## -- " << sTestName << std::endl;
		result.GetError().Log();
	}
	else
	{
		std::cout << "PASS -- " << sTestName << std::endl;
	}
}

static inline bool farb_print (bool success, std::string sTestName)
{
	if (success)
		std::cout << "PASS -- ";
	else
		std::cout << "## FAIL ## -- ";
	std::cout << sTestName << std::endl;
	return success;
}

static inline bool farb_print (bool success, std::string sTrue, std::string sFalse)
{
	if (success && !sTrue.empty())
		std::cout << "PASS -- " << sTrue << std::endl;
	else if (!success && !sFalse.empty())
		std::cout << "## FAIL ## -- " << sFalse << std::endl;
	return success;
}

class ITest
{
public:
	virtual bool RunTests() const = 0;
};

} // namespace Tests

} // namespace Farb

#endif // REGISTER_TEST_H
