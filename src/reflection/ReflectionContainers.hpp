#ifndef FARB_REFLECTION_ARRAY_H
#define FARB_REFLECTION_ARRAY_H

#include <limits.h>
#include <vector>

#include "ReflectionDeclare.h"
#include "ReflectionDefine.hpp"

namespace Farb
{

namespace Reflection
{

template<typename TVal>
static auto vectorTypeInfo = TypeInfoArray<std::vector<TVal>, TVal>(
	std::string("std::vector<" + GetTypeInfo<TVal>()->GetName() + ">"));


template<typename TVal>
struct TemplatedTypeInfo<std::vector<TVal>>
{
	static TypeInfo* GetTypeInfo()
	{
		return &vectorTypeInfo<TVal>;
	}
};

template<typename TKey, typename TVal>
static auto unorderedMapTypeInfo = TypeInfoTable

} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_ARRAY_H
