
#include <iostream>

#include "./reflection/TestReflectEnum.hpp"
#include "./reflection/TestReflectStruct.hpp"
#include "./reflection/TestReflectContainers.hpp"
#include "./reflection/TestReflectWrappers.hpp"
#include "./serialization/TestDeserialize.hpp"
#include "./interface/TestUITree.hpp"
//#include "./utils/TestMapReduce.hpp"
#include "./core/TestErrorOr.hpp"
/*
g++ -std=c++17 -Wfatal-errors RunTests.cpp -g && ./a.out;
*/

using namespace Farb::Tests;

int main(void)
{
	std::cout << "Beginning Tests" << std::endl;
	
	bool success = Run<
		TestReflectEnum,
		TestReflectStruct,
		TestReflectContainers,
		TestReflectWrappers,
		TestDeserialize,
		TestUITree,
		//TestMapReduce,
		TestErrorOr>();
	
	std::cout << "All Tests Passed" << std::endl;
	if (success) return 0;
	return 1;
}
