#ifndef FARB_REFLECTION_ARRAY_H
#define FARB_REFLECTION_ARRAY_H

#include <limits.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "ReflectionDeclare.h"
#include "ReflectionDefine.hpp"

namespace Farb
{

namespace Reflection
{

template<typename TVal>
struct TemplatedTypeInfo<std::vector<TVal> >
{
	// rmf note: this childName parameter was introduced as a way to avoid
	// circular dependencies for UI::Node.children which is a std::vector<UI::Node>
	// this feels a little hacky
	static TypeInfo* Get(HString childName = GetTypeInfo<TVal>()->GetName())
	{
		static auto typeInfo = TypeInfoArray<std::vector<TVal>, TVal>::template Construct<std::vector<TVal> >(
	HString("std::vector<" + childName + ">"));

		return &typeInfo;
	}
};

// rmf todo: [N] types could be made more efficient if they were done in place
// rather than reflected as std::vector

template<typename TVal, size_t NSize>
struct ArrayConverter : public Functor<ErrorOr<TVal[NSize]>, const std::vector<TVal> & >
{
	virtual ErrorOr<TVal[NSize]> operator()(const std::vector<TVal> & in) override
	{
		if (in.size() != NSize)
		{
			return Error("Fixed legnth array expects "
				+ ToString(NSize)
				+ " but was given "
				+ ToString(in.size()));
		}

		TVal ret[NSize];
		for (int i = 0; i < NSize; ++i)
		{
			ret[i] = in[i];
		}
		return ret;
	}
};

template<typename TVal, size_t NSize>
struct TemplatedTypeInfo<TVal[NSize]>
{
	static TypeInfo* Get()
	{
		static ArrayConverter<TVal, NSize> converter;
		static auto typeInfo = TypeInfoAs<TVal[NSize], std::vector<TVal> >(
			GetTypeInfo<TVal>()->GetName() + "[" + ToString(NSize) + "]",
			converter);
		return &typeInfo;
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
struct TemplatedTypeInfo<std::unordered_set<TVal> >
{
	static TypeInfo* Get()
	{
		static auto setTypeInfo = TypeInfoArray<std::unordered_set<TVal>, TVal>(
			HString("std::unordered_set<" + Reflection::GetTypeInfo<TVal>()->GetName() + ">"),
			SetTypeInfoValueBuffer<TVal>::BoundsCheck,
			SetTypeInfoValueBuffer<TVal>::At,
			SetTypeInfoValueBuffer<TVal>::PushBackDefault,
			SetTypeInfoValueBuffer<TVal>::ArrayEnd
		);

		return &setTypeInfo;
	}
};

template<typename TKey, typename TVal>
struct TemplatedTypeInfo<std::unordered_map<TKey, TVal> >
{
	static TypeInfo* Get()
	{
		static auto mapTypeInfo = TypeInfoTable<std::unordered_map<TKey, TVal>, TKey, TVal>(
			HString(
				"std::unordered_map<"
				+ Reflection::GetTypeInfo<TKey>()->GetName()
				+ ", "
				+ Reflection::GetTypeInfo<TVal>()->GetName()
				+ ">")
		);

		return &mapTypeInfo;
	}
};


} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_ARRAY_H
