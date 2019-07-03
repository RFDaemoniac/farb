#ifndef FARB_REFLECTION_DEFINE_H
#define FARB_REFLECTION_DEFINE_H

#include <memory>
#include <iostream>
#include <limits.h>
#include <type_traits>
#include <unordered_map>

#include "ReflectionDeclare.h"


namespace Farb
{

namespace Reflection
{

template<typename T>
struct AssignFunction
{
	template<typename TArg>
	bool Call(T& object, TArg value) const;

	virtual ~AssignFunction() = default;
};

template<typename T, typename TArg>
struct AssignFunctionTyped : public AssignFunction<T>
{
	static_assert(
		std::is_same<TArg, bool>::value
		|| std::is_same<TArg, uint>::value
		|| std::is_same<TArg, int>::value
		|| std::is_same<TArg, float>::value
		|| std::is_same<TArg, std::string>::value,
		"AssignFunctionTyped can only accept values of type bool, uint, int, float, or std::string");
	friend class AssignFunction<T>;
protected:
	bool (*pAssign)(T&, TArg);

public:
	AssignFunctionTyped(bool (*pAssign)(T&, TArg))
		: pAssign(pAssign)
	{ }
};

template<typename T, typename TArg>
struct AssignFunctionTypedMember : public AssignFunction<T>
{
	static_assert(
		std::is_same<TArg, bool>::value
		|| std::is_same<TArg, uint>::value
		|| std::is_same<TArg, int>::value
		|| std::is_same<TArg, float>::value
		|| std::is_same<TArg, std::string>::value,
		"AssignFunctionTypedMember can only accept values of type bool, uint, int, float, or std::string");
	friend class AssignFunction<T>;
protected:
	bool (T::*pAssign)(TArg);

public:
	AssignFunctionTypedMember(bool (T::*pAssign)(T&, TArg))
		: pAssign(pAssign)
	{ }
};

template<typename T>
template<typename TArg>
bool AssignFunction<T>::Call(T& object, TArg value) const
{
	if constexpr (!std::is_fundamental<T>::value)
	{
		auto pAssignMemberWrapper = dynamic_cast<const AssignFunctionTypedMember<T, TArg>*>(this);
		if (pAssignMemberWrapper != nullptr)
		{
			auto pAssignMember = pAssignMemberWrapper->pAssign;
			return (object.*pAssignMember)(value);
		}
	}
	auto pAssignWrapper = dynamic_cast<const AssignFunctionTyped<T, TArg>*>(this);
	if (pAssignWrapper != nullptr)
	{
		return pAssignWrapper->pAssign(object, value);
	}
	return false;
}

template<typename T>
struct ExtractFunctionTypes
{ };

template<typename C, typename R, typename A>
struct ExtractFunctionTypes<R(C::*)(A)>
{
	using IsMember = std::true_type;
	using Class = C;
	using Return = R;
	using Param = A;
};

template<typename C, typename R, typename A>
struct ExtractFunctionTypes<R(*)(C&, A)>
{
	using IsMember = std::false_type;
	using Class = C;
	using Return = R;
	using Param = A;
};

template<typename T>
struct TypeInfoCustomLeaf : public TypeInfo
{
protected:
	std::unordered_map<std::string, std::shared_ptr<AssignFunction <T> > > tpAssignFunctions;

public:
	template <typename ... TArgs>
	static TypeInfoCustomLeaf Construct(HString name, TArgs ... args)
	{
		auto leafTypeInfo = TypeInfoCustomLeaf(name);
		leafTypeInfo.RegisterAssignFunctions(args...);
		return leafTypeInfo;
	}

	virtual bool AssignBool(byte* obj, bool value) const override
	{
		return Assign(obj, value);
	}

	virtual bool AssignUInt(byte* obj, uint value) const override
	{
		return Assign(obj, value);
	}

	virtual bool AssignInt(byte* obj, int value) const override
	{
		return Assign(obj, value);
	}

	virtual bool AssignFloat(byte* obj, float value) const override
	{
		return Assign(obj, value);
	}

	virtual bool AssignString(byte* obj, std::string value) const override
	{
		return Assign(obj, value);
	}

protected:
	TypeInfoCustomLeaf(HString name)
		: TypeInfo(name)
		, tpAssignFunctions()
	{ }

	template <typename TArg>
	bool Assign(byte* obj, TArg value) const
	{
		auto typeName = GetTypeInfo<TArg>()->GetName();
		const auto iter = tpAssignFunctions.find(typeName);
		if (iter == tpAssignFunctions.end())
		{
			// we do not have an assign function that takes this type
			return false;
		}
		T* t = reinterpret_cast<T*>(obj);
		return iter->second->Call(*t, value);
	}

private:
	template <typename TFunc>
	void RegisterAssignFunctions(TFunc pAssign)
	{
		using TArg = typename ExtractFunctionTypes<TFunc>::Param;
		if constexpr (ExtractFunctionTypes<TFunc>::IsMember::value)
		{
			using C = typename ExtractFunctionTypes<TFunc>::Class;
			static_assert(
				std::is_same<T, C>::value,
				"member function for TypeInfoCustomLeaf belongs to another class");
			auto typeName = GetTypeInfo<TArg>()->GetName();
			tpAssignFunctions.emplace(
				typeName,
				new AssignFunctionTypedMember<T, TArg>(pAssign));
		}
		else
		{
			auto typeName = GetTypeInfo<TArg>()->GetName();
			tpAssignFunctions.emplace(
				typeName,
				new AssignFunctionTyped<T, TArg>(pAssign));
		}
	}

	template <typename TFunc, typename ... Rest>
	void RegisterAssignFunctions(TFunc pAssign, Rest ... rest)
	{
		RegisterAssignFunctions(pAssign);
		RegisterAssignFunctions(rest...);
	}
};

template<typename T>
struct TypeInfoEnum : public TypeInfo
{
protected:
	std::vector<std::pair<HString, int> > vValues;

public:

	TypeInfoEnum(HString name, std::vector<std::pair<HString, int> > vEnumValues)
		: TypeInfo(name)
		, vValues(vEnumValues)
	{

	}

	virtual bool AssignInt(byte* obj, int value) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		for (const auto & pair : vValues)
		{
			if (pair.second == value)
			{
				*t = static_cast<T>(pair.second);
				return true;
			}
		}
		return false;
	}

	virtual bool AssignUInt(byte* obj, uint value) const override
	{
		if (value > INT_MAX)
		{
			return false;
		}
		return AssignInt(obj, static_cast<int>(value));
	}

	virtual bool AssignString(byte* obj, std::string value) const override
	{
		HString sValue = value;
		T* t = reinterpret_cast<T*>(obj);
		for (const auto & pair : vValues)
		{
			if (pair.first == sValue)
			{
				*t = static_cast<T>(pair.second);
				return true;
			}
		}
		return false;
	}
};

/*
// rmf todo: consider using this kind of class to unify the api
// if multiple types don't fit.
// key thing that seems like it's not fitting is a hash set, which
// can't really push back a default, since changing the value changes the location
// this exposes a weakness in the in-place reflection style I've been going for
// but we could get super complex with this interface allowing it to keep track of
// an individual item being reflected, we would need to add calls when we're moving
// on to the next item. Maybe when the next PushBackDefault is called or 
template <typename T, typename TVal>
struct ArrayInterface
{
	static bool BoundsCheck(T& obj, int index)
	{
		return index >= 0 && obj.size() <= index;
	}

	static TVal& At(T& obj, int index)
	{
		return t[index];
	}

	static void PushBackDefault(T& obj)
	{
		t.push_back(TVal());
	}
}

template<typename TVal>
struct ArrayInterface<std::unordered_set<TVal>, TVal>
{
	static std::unordered_map<std::unordered_set<TVal>*, TVal*> current;

	static void PushBackDefault(T& obj)
	{
		if (current.contains(&obj) && current[&obj] != nullptr)
		{
			t.push_back(*(current[&obj]));
			delete current[&obj];
		}
		current[&obj] = new TVal();
	}

	static void ArrayEnd(T& obj)
	{
		if (current.contains(&obj) && current[&obj] != nullptr)
		{
			t.push_back(*(current[&obj]));
			delete current[&obj];
			current.erase(&obj);
		}
	}
	
}
*/

template<typename T, typename TVal>
struct TypeInfoArray : public TypeInfo
{
	TypeInfoArray(HString name)
		: TypeInfo(name)
	{ }

	// rmf todo: @implement an array of pointers to objects that have derived types
	// will be a little trickier to set up when to create something and what to create
	// if copied/inherited/included you can just point to the same object
	// until we overwrite a different key, then you need to new and use the copy constructor
	// but that logic should be in the deserialization, not in reflection
	virtual ErrorOr<ReflectionObject> GetAtIndex(
		byte* obj,
		int index) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		if (t->size() <= index)
		{
			return Error(
				name
				+ " array GetAtIndex "
				+ std::to_string(index)
				+ " is out of bounds, "
				+ std::to_string(t->size())
				+ ".");
		}
		return ReflectionObject::Construct((*t)[index]);
	}

	virtual bool PushBackDefault(byte* obj) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		t->push_back(TVal());
		return true;
	}
};

template<typename T, typename TKey, typename TVal>
struct TypeInfoTable : public TypeInfo
{
	virtual ErrorOr<ReflectionObject> GetAtKey(
		byte* obj,
		HString name) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		TKey key = TKey(name);

		if (!t.contains(key))
		{
			return Error(this->name
				+ " table GetAtKey "
				+ name
				+ " failed. No entry exists with that name.");
		}

		TVal& value = t.at(key);
		return ReflectionObject::Construct(value);
	}

	virtual bool InsertKey(byte* obj, HString name) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		TKey key = TKey(name);

		if (t.contains(key))
		{
			return false;
		}
		return t.insert({key, TVal()}).second;
	}
};

template<typename T>
struct TypeInfoStruct;

template<typename T>
struct MemberInfo
{
	friend class TypeInfoStruct<T>;
protected:
	HString name;
	TypeInfo* typeInfo;

	MemberInfo(HString name, TypeInfo* typeInfo)
		: name(name)
		, typeInfo(typeInfo)
	{}

public:
	virtual ~MemberInfo() { }

public:
	virtual byte* GetLocation(T* t) const = 0;
};

template<typename T, typename TMem>
struct MemberInfoTyped : public MemberInfo<T>
{
protected:
	TMem T::* location;

public:
	MemberInfoTyped(HString name, TMem T::* location)
		: MemberInfo<T>(name, GetTypeInfo<TMem>())
		, location(location)
	{}

	virtual byte* GetLocation(T* t) const override
	{
		TMem* pMem = &(t->*location);
		return reinterpret_cast<byte*>(pMem);
	}
};

template<typename T>
struct TypeInfoStruct : public TypeInfo
{
	// rmf note: at first all TypeInfo would have the opportunity for a parent type
	// but I figured it was mostly unecessary for the others. Feel free to change in the future.
	TypeInfo* parentType;
	std::vector<MemberInfo<T>*> vMembers;

	TypeInfoStruct(
		HString name,
		TypeInfo* parentType,
		std::vector<MemberInfo<T>*> members)
		: TypeInfo(name)
		, parentType(parentType)
		, vMembers(members)
	{}

	~TypeInfoStruct()
	{
		for (auto pMember : vMembers)
		{
			delete pMember;
		}
	}

	virtual ErrorOr<ReflectionObject> GetAtKey(
		byte* obj,
		HString name) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		for (auto member : vMembers)
		{
			if (member->name == name)
			{
				return ReflectionObject(member->GetLocation(t), member->typeInfo);
			}
		}

		if (parentType != nullptr)
		{
			return parentType->GetAtKey(obj, name);
		}
		return Error(
			this->name
			+ " struct GetAtKey "
			+ name
			+ " failed. No member exists with that name.");
	}
};


} // namespace Reflection

} // namespace Farb


#endif // FARB_REFLECTION_DEFINE_H
