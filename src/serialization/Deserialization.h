#ifndef FARB_DESERIALIZATION_H
#define FARB_DESERIALIZATION_H

#include <string>

#include "../reflection/ReflectionDeclare.h"

namespace Farb
{

bool DeserializeString(std::string input, Reflection::ReflectionObject reflect);
bool DeserializeFile(std::string input, Reflection::ReflectionObject reflect);

} // namespace Farb

#endif // FARB_DESERIALIZATION_H