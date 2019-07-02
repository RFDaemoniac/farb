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
static TypeInfoArray<std::vector<TVal>, TVal> vectorTypeInfo = TypeInfoArray<std::vector<TVal>, TVal>(
	std::string("std::vector<" + GetTypeInfo<TVal>()->GetName() + ">"));


template<typename TVal>
struct TypeInfoHolder<std::vector<TVal>>
{
	static TypeInfo* GetTypeInfo()
	{
		return &vectorTypeInfo<TVal>;
	}
};

} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_ARRAY_H
