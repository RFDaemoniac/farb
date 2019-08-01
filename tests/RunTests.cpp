
#include <iostream>

#include "./reflection/TestReflectEnum.hpp"
#include "./reflection/TestReflectStruct.hpp"
#include "./reflection/TestReflectContainers.hpp"
#include "./reflection/TestReflectWrappers.hpp"
#include "./serialization/TestDeserialize.hpp"
#include "./interface/TestUITree.hpp"
/*
g++ -std=c++17 -Wfatal-errors RunTests.cpp -g && ./a.out;
*/

using namespace Farb::Tests;

template<typename TTest, typename ... TTests>
bool Run()
{
    // ... copies the previous statement for each of TTests
    // comma operator combines them in
    bool success = true;
    ((success &= TTests().RunTests()),...);
    return success;
}

int main(void)
{
	std::cout << "Beginning Tests" << std::endl;
	
	bool success = Run<
		TestReflectEnum,
		TestReflectStruct,
		TestReflectContainers,
		TestReflectWrappers,
		TestDeserialize,
		TestUITree>();
	
	std::cout << "All Tests Passed" << std::endl;
	if (success) return 0;
	return 1;
}
