#ifndef FARB_DESERIALIZATION_H
#define FARB_DESERIALIZATION_H

#include <string>

#include "../reflection/ReflectionDeclare.h"

namespace Farb
{

template<typename T>
bool DeserializeFile(std::string filePath, T& object)
{
	DeserializeFile(filePath, Reflection::Reflect(object));
}

template<typename T>
bool DeserializeString(std::string input, T& object)
{
	DeserializeString(input, Reflection::Reflect(object));
}

bool DeserializeString(std::string input, Reflection::ReflectionObject reflect);
bool DeserializeFile(std::string input, Reflection::ReflectionObject reflect);

} // namespace Farb

#endif // FARB_DESERIALIZATION_H