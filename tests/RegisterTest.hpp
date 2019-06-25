#ifndef REGISTER_TEST_H
#define REGISTER_TEST_H

#include <assert>
#include <iostream>
#include <string>

namespace Farb
{

namespace Tests
{

static inline bool assert_print (bool success, std::string sTestName)
{
	if (success)
		std::cout << "PASS -- ";
	else
		std::cout << "## FAIL ## -- ";
	std::cout << sTestName << std::endl;
	assert(success);
	return success;
}

static inline bool assert_print (bool success, std::string sTrue, std::string sFalse)
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
