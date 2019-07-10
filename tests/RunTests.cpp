
#include <iostream>

#include "./reflection/TestReflectEnum.hpp"
#include "./reflection/TestReflectStruct.hpp"
#include "./reflection/TestReflectContainers.hpp"
#include "./reflection/TestReflectWrappers.hpp"
#include "./serialization/TestDeserialize.hpp"
#include "./interface/TestDeserializeUITree.hpp"
/*
g++ -std=c++17 -Wfatal-errors RunTests.cpp -g && ./a.out;
*/

using namespace Farb::Tests;

template<typename TTest, typename ... TRest>
bool Run()
{
	if constexpr(sizeof...(TRest) == 0)
	{
		return TTest().RunTests();
	}
	else
	{
		return TTest().RunTests() && Run<TRest...>();
	}
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
		TestDeserializeUITree>();
	
	std::cout << "All Tests Passed" << std::endl;
	return success;
}
