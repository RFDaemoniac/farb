
#include <iostream>

#include "./reflection/TestReflectEnum.hpp"
#include "./reflection/TestReflectStruct.hpp"
#include "./reflection/TestReflectContainers.hpp"
/*
g++ -std=c++17 -Wfatal-errors RunTests.cpp -g && ./a.out;
*/

using namespace Farb::Tests;

int main(void)
{
	std::cout << "Beginning Tests" << std::endl;
	TestReflectEnum testReflectEnum;
	bool success = testReflectEnum.RunTests();
	TestReflectStruct testReflectStruct;
	success = testReflectStruct.RunTests();
	TestReflectContainers testReflectContainers;
	success = testReflectContainers.RunTests();
	std::cout << "All Tests Passed" << std::endl;
	return success;
}
