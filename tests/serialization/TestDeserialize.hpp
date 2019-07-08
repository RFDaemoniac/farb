#ifndef TEST_DESERIALIZE_HPP
#define TEST_DESERIALIZE_HPP

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDeclare.h"
#include "../../src/reflection/ReflectionBasics.hpp"
#include "../../src/reflection/ReflectionWrappers.hpp"
#include "../reflection/TestReflectDefinitions.hpp"
#include "../../src/serialization/Deserialization.h"

namespace Farb
{
using namespace Reflection;

namespace Tests
{

template<typename T>
void TestDeserializeValue(const T& initial, const T& expected, std::string jsonString)
{
	T test = initial;
	assert(test == initial);
	bool success = DeserializeString(jsonString, Reflect(test));
	farb_print(success && test == expected,
		"deserialize value " + GetTypeInfo<T>()->GetName());
	assert(success && test == expected);
}

template<typename T, typename TKey, typename TVal>
void TestDeserializeContainer(
	std::string jsonString,
	int expectedSize,\
	const std::pair<TKey, TVal>& expected1,
	const std::pair<TKey, TVal>& expected2)
{
	T test;
	assert(test.size() == 0);
	bool success = DeserializeString(jsonString, Reflect(test));
	success = success
		&& test.size() == expectedSize
		&& test[expected1.first] == expected1.second
		&& test[expected2.first] == expected2.second;
	farb_print(success, "deserialize container " + GetTypeInfo<T>()->GetName());
	assert(success);
}

class TestDeserialize : public ITest
{
public:
	virtual bool RunTests() const override
	{
		std::cout << "Deserialize" << std::endl;

		TestDeserializeValue<bool>(false, true, "true");
		TestDeserializeValue<uint>(0, 1, "1");
		TestDeserializeValue<int>(0, -1, "-1");
		TestDeserializeValue<float>(0.0, 10.5, "10.5");
		TestDeserializeValue<std::string>("", "AB C", "\"AB C\"");
		TestDeserializeValue<ExampleEnum>(ExampleEnum::Zero, ExampleEnum::One, "\"One\"");
		TestDeserializeValue<ExampleNamedTypeInt>(
			ExampleNamedTypeInt(0),
			ExampleNamedTypeInt(1),
			"1");
		TestDeserializeValue<ExampleBaseStruct>(
			ExampleBaseStruct(ExampleEnum::Zero, 0),
			ExampleBaseStruct(ExampleEnum::One, 1),
			"{\"e1\": \"One\", \"i2\": 1 }");

		TestDeserializeContainer<std::vector<int>, int, int>(
			"[0,1, 2, 3,4]",
			5, {0, 0}, {4, 4});

		TestDeserializeContainer<std::unordered_map<std::string, int>, std::string, int>(
			"{\"Zero\": 0, \"One\": 1, \"Two\": 2}",
			3, {"Zero", 0}, {"Two", 2});

		TestDeserializeContainer<std::vector<ExampleBaseStruct>, int, ExampleBaseStruct>(
			"["
			"	{\"e1\": \"One\", \"i2\": 1 },"
			"	{\"e1\": \"Two\", \"i2\": 2 }"
			"]",
			2,
			{0, ExampleBaseStruct(ExampleEnum::One, 1)},
			{1, ExampleBaseStruct(ExampleEnum::Two, 2)});

		ExampleBaseStruct testStructFromFile;
		bool success = DeserializeFile("./tests/files/TestExampleBaseStruct.json", Reflect(testStructFromFile));
		success = success && testStructFromFile == ExampleBaseStruct(ExampleEnum::One, 1);
		farb_print(success, "deserialize from file value " + GetTypeInfo<ExampleBaseStruct>()->GetName());
		assert(success);

		return true;
	}
};

} // namespace Tests

} // namespace Farb

#endif // TEST_DESERIALIZE_HPP
