
#include <iostream>

#include "./reflection/TestReflectEnum.hpp"
#include "./reflection/TestReflectStruct.hpp"
/*
g++ -std=c++11 -Wfatal-errors RunTests.cpp
*/

using namespace Farb::Tests;

int main(void)
{
	std::cout << "Beginning Tests" << std::endl;
	TestReflectEnum testReflectEnum;
	bool success = testReflectEnum.RunTests();
	TestReflectStruct testReflectStruct;
	success = testReflectStruct.RunTests();
	std::cout << "All Tests Passed" << std::endl;
	return success;
}
