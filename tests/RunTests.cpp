
#include <iostream>

#include "./reflection/TestReflectEnum.hpp"
/*
g++ -std=c++11 RunTests.cpp
*/

using namespace Farb::Tests;

int main(void)
{
	std::cout << "Beginning Tests" << std::endl;
	TestReflectEnum testReflectEnum;
	bool success = testReflectEnum.RunTests();
	std::cout << "All Tests Passed" << std::endl;
	return success;
}
