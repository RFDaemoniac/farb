#ifndef FARB_REFLECTION_BASICS_H
#define FARB_REFLECTION_BASICS_H

#include "ReflectionDeclare.h"

namespace Farb
{

namespace Reflection
{

template <> TypeInfo* GetTypeInfo<bool>();
template <> TypeInfo* GetTypeInfo<uint>();
template <> TypeInfo* GetTypeInfo<int>();
template <> TypeInfo* GetTypeInfo<float>();
template <> TypeInfo* GetTypeInfo<std::string>();

} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_BASICS_H