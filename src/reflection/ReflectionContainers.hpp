#ifndef FARB_REFLECTION_ARRAY_H
#define FARB_REFLECTION_ARRAY_H

#include <limits.h>
#include <vector>
#include <unordered_set>

#include "ReflectionDeclare.h"
#include "ReflectionDefine.hpp"

namespace Farb
{

namespace Reflection
{

template<typename TVal>
static auto vectorTypeInfo = TypeInfoArray<std::vector<TVal>, TVal>::template Construct<std::vector<TVal> >(
	HString("std::vector<" + GetTypeInfo<TVal>()->GetName() + ">"));


template<typename TVal>
struct TemplatedTypeInfo<std::vector<TVal> >
{
	static TypeInfo* GetTypeInfo()
	{
		return &vectorTypeInfo<TVal>;
	}
};

// this is to compensate for the fact that deserialization happens in-place right now
// but that doesn't work for sets, which need to insert after the object has been constructred
template <typename TVal>
struct SetTypeInfoValueBuffer
{
	// one buffer slot per set
	// rmf todo: @implement this is not threadsafe
	static std::unordered_map<std::unordered_set<TVal>*, TVal*> current;

	static bool BoundsCheck(std::unordered_set<TVal>& obj, int index)
	{
		// only the pending item is valid for reflection access at the moment
		bool isNextIndex = (index >= 0) && (obj.size() == index);
		bool bufferHasValue = current.count(&obj) && current[&obj] != nullptr;
		return isNextIndex && bufferHasValue;
	}

	static TVal& At(std::unordered_set<TVal>& obj, int index)
	{
		// assume bounds check has already been performed
		return *(current[&obj]);
	}

	static bool PushBackDefault(std::unordered_set<TVal>& obj)
	{
		bool bufferHasValue = current.count(&obj) && current[&obj] != nullptr;
		if (bufferHasValue)
		{
			obj.insert(*(current[&obj]));
			*(current[&obj]) = TVal();
		}
		else
		{
			current[&obj] = new TVal();
		}
		return true;
	}

	static bool ArrayEnd(std::unordered_set<TVal>& obj)
	{
		bool bufferHasObject = current.count(&obj);
		bool bufferHasValue = bufferHasObject && current[&obj] != nullptr;
		if (bufferHasValue)
		{
			obj.insert(*(current[&obj]));
			delete current[&obj];
		}
		if (bufferHasObject)
		{
			current.erase(&obj);
		}
		return true;
	}
};

template<typename TVal>
std::unordered_map<std::unordered_set<TVal>*, TVal*> SetTypeInfoValueBuffer<TVal>::current;

template<typename TVal>
static auto setTypeInfo = TypeInfoArray<std::unordered_set<TVal>, TVal>(
	HString("std::unordered_set<" + GetTypeInfo<TVal>()->GetName() + ">"),
	SetTypeInfoValueBuffer<TVal>::BoundsCheck,
	SetTypeInfoValueBuffer<TVal>::At,
	SetTypeInfoValueBuffer<TVal>::PushBackDefault,
	SetTypeInfoValueBuffer<TVal>::ArrayEnd
);

template<typename TVal>
struct TemplatedTypeInfo<std::unordered_set<TVal> >
{
	static TypeInfo* GetTypeInfo()
	{
		return &setTypeInfo<TVal>;
	}
};

} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_ARRAY_H