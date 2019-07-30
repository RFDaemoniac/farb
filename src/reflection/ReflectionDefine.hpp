#ifndef FARB_REFLECTION_DEFINE_H
#define FARB_REFLECTION_DEFINE_H

#include <memory>
#include <iostream>
#include <limits.h>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "../utils/MapReduce.hpp"
#include "../utils/TypeInspection.hpp"
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
	friend struct AssignFunction<T>;
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
	friend struct AssignFunction<T>;
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

	virtual ErrorOr<std::string> ToString(byte* obj) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		if constexpr(std::is_same<bool, T>::value)
		{
			bool b = static_cast<bool>(t);
			return b ? std::string("true") : std::string("false");
		}
		else if constexpr(std::is_integral<T>::value || std::is_floating_point<T>::value)
		{
			return std::to_string(*t);
		}
		else if constexpr(std::is_same<std::string, T>::value)
		{
			return ErrorOr<std::string>(*t);
		}
		else
		{
			return Error(name + " ToString not implemented");
		}
	}

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

protected:
	TypeInfoCustomLeaf(HString name)
		: TypeInfo(name)
		, tpAssignFunctions()
	{ }

private:
	template <typename TFunc>
	void RegisterAssignFunctions(HString typeName, TFunc pAssign)
	{
		using TArg = typename ExtractFunctionTypes<TFunc>::Param;
		if constexpr (ExtractFunctionTypes<TFunc>::IsMember::value)
		{
			using C = typename ExtractFunctionTypes<TFunc>::Class;
			static_assert(
				std::is_same<T, C>::value,
				"member function for TypeInfoCustomLeaf belongs to another class");
			tpAssignFunctions.emplace(
				typeName,
				new AssignFunctionTypedMember<T, TArg>(pAssign));
		}
		else
		{
			tpAssignFunctions.emplace(
				typeName,
				new AssignFunctionTyped<T, TArg>(pAssign));
		}
	}


	// rmf note: this could cause a link lock if the types are interdependent
	// so you can specify a type name to get around this
	// see the overload that accepts an HString typeName
	template <typename TFunc>
	void RegisterAssignFunctions(TFunc pAssign)
	{
		using TArg = typename ExtractFunctionTypes<TFunc>::Param;
		/*
		rmf todo: consider trying to evaluate whether this is safe
		so that we can give a better error message than just a linker deadlock...
		static_assert(
			std::is_same<TArg, T>::value
			|| std::is_same<T,
			"assignment functions whose value type does not match the target type must specify the typename of their value");
		*/
		HString typeName = [&]
		{
			if constexpr(std::is_same<TArg, T>::value)
			{
				return name;
			} else {
				return GetTypeInfo<TArg>()->GetName();
			}
		}();

		AssignFunction<T>* pAssignWrapper = [&]
		{
			if constexpr(ExtractFunctionTypes<TFunc>::IsMember::value)
			{
				using C = typename ExtractFunctionTypes<TFunc>::Class;
				static_assert(
					std::is_same<T, C>::value,
					"member function for TypeInfoCustomLeaf belongs to another class");
				return new AssignFunctionTypedMember<T, TArg>(pAssign);
			}
			else
			{
				return new AssignFunctionTyped<T, TArg>(pAssign);
			}
		}();
		tpAssignFunctions.emplace(typeName, pAssignWrapper);
	}


	template <typename TFunc, typename ... Rest>
	void RegisterAssignFunctions(HString typeName, TFunc pAssign, Rest ... rest)
	{
		RegisterAssignFunctions(typeName, pAssign);
		RegisterAssignFunctions(rest...);
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

	virtual ErrorOr<std::string> ToString(byte* obj) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		for (const auto & pair : vValues)
		{
			if (static_cast<T>(pair.second) == *t)
			{
				return ErrorOr<std::string>(pair.first);
			}
		}
		return std::to_string(static_cast<int>(*t));
	}
};

// rmf todo: 
// key thing that seems like it's not fitting is a hash set, which
// can't really push back a default, since changing the value changes the location
// this exposes a weakness in the in-place reflection style I've been going for
// but we could get super complex with this interface allowing it to keep track of
// an individual item being reflected, we would need to add calls when we're moving
// on to the next item. Maybe when the next PushBackDefault is called or ArrayEnd
// we could also consider having an alternative interface for copying values
// rather than reflecting in place

// should also consider fixed length arrays that can't use PushBackDefault


template<typename T, typename TVal>
struct TypeInfoArray : public TypeInfo
{
	using FBoundsCheck = bool (*)(T& obj, int index);
	using FAt = TVal& (*)(T& obj, int index);
	using FPushBackDefault = bool (*)(T& obj);
	using FArrayEnd = bool (*)(T&obj);

private:
	FBoundsCheck pBoundsCheck;
	FAt pAt;
	FPushBackDefault pPushBackDefault;
	FArrayEnd pArrayEnd;

public:
	TypeInfoArray(
		HString name,
		FBoundsCheck pBoundsCheck,
		FAt pAt,
		FPushBackDefault pPushBackDefault,
		FArrayEnd pArrayEnd)
		: TypeInfo(name)
		, pBoundsCheck(pBoundsCheck)
		, pAt(pAt)
		, pPushBackDefault(pPushBackDefault)
		, pArrayEnd(pArrayEnd)
	{ }

	template<typename TDefaultInterfaceArray >
	static TypeInfoArray Construct(
		HString name)
	{
		static_assert(
			std::is_same<T, TDefaultInterfaceArray>::value,
			"TypeInfoArray should not be constructed with another type as the default interface")	;
		return TypeInfoArray(
			name,
			static_cast<FBoundsCheck>([](T& obj, int index )
			{
				return index >= 0 && obj.size() > index;
			}),
			static_cast<FAt>([](T& obj, int index) -> TVal& { return obj[index]; }),
			static_cast<FPushBackDefault>([](T& obj) { obj.push_back(TVal()); return true; }),
			static_cast<FArrayEnd>([](T& obj) { return true; }));
	}

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
		if (!pBoundsCheck(*t, index))
		{
			return Error(
				name
				+ " array GetAtIndex "
				+ std::to_string(index)
				+ " is out of bounds, "
				+ std::to_string(t->size())
				+ ".");
		}
		return Reflect(pAt(*t, index));
	}

	virtual bool PushBackDefault(byte* obj) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		return pPushBackDefault(*t);
	}

	virtual bool ArrayEnd(byte* obj) const override
	{
		if (pArrayEnd == nullptr) return true;
		T* t = reinterpret_cast<T*>(obj);
		return pArrayEnd(*t);
	}
};

template<typename T, typename TKey, typename TVal>
struct TypeInfoTable : public TypeInfo
{
	TypeInfoTable(HString name)
		: TypeInfo(name)
	{ }

	virtual ErrorOr<ReflectionObject> GetAtKey(
		byte* obj,
		HString name) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		TKey key = TKey(name);

		if (!t->count(key))
		{
			return Error(this->name
				+ " table GetAtKey "
				+ name
				+ " failed. No entry exists with that name.");
		}

		TVal& value = t->at(key);
		return Reflect(value);
	}

	virtual bool InsertKey(byte* obj, HString name) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		TKey key = TKey(name);

		if (t->count(key))
		{
			return false;
		}
		return t->insert({key, TVal()}).second;
	}
};


// rmf note: pointers seem like they share a similar interface with this temp location
// except instead of getting values[t] you dereference the pointer...

template <typename T, typename TDeserialize>
struct TypeInfoAs : public TypeInfo
{
	TypeInfo * typeInfo;

	// rmf todo: consider switching converter to take in T& for in-place manipulation
	// rather than relying on a copy constructor
	// I generally don't like out parameters though...
	using Converter = Functor<ErrorOr<T>, const TDeserialize &>;
	Converter & converter;

	std::unique_ptr<std::unordered_map<T*, TDeserialize>> values;

	TypeInfoAs(HString name, Converter & converter)
		: TypeInfo(name)
		, typeInfo(GetTypeInfo<TDeserialize>())
		, converter(converter)
		, values(new std::unordered_map<T*, TDeserialize>)
	{ }

protected:
	
	template<typename ... TArgs>
	ErrorOr<Success> PassThrough(ErrorOr<Success> (TypeInfo::*function)(byte* obj, TArgs...) const, byte* obj, TArgs... args) const
	{
		T* t = reinterpret_cast<T*>(obj);
		byte* temp = reinterpret_cast<byte*>(&values[t]);
		CHECK_RETURN((typeInfo->*function)(temp, args...));
		(*t) = CHECK_RETURN(converter(values[t]));
		values.erase(t);
		return Success();
	}
	

	template<typename ... TArgs>
	bool PassThrough(bool (TypeInfo::*function)(byte* obj, TArgs...) const, byte* obj, TArgs... args) const
	{
		T* t = reinterpret_cast<T*>(obj);
		byte* temp = reinterpret_cast<byte*>(&(*values)[t]);
		bool success = (typeInfo->*function)(temp, args...);
		if (!success) return false;
		auto result = converter((*values)[t]);
		if (result.IsError())
		{
			result.GetError().Log();
			return false;
		}
		(*t) = result.GetValue();
		values->erase(t);
		return true;
	}

	virtual bool AssignBool(byte* obj, bool value) const override
	{
		return PassThrough(&TypeInfo::AssignBool, obj, value);
	}

	virtual bool AssignUInt(byte* obj, uint value) const override
	{
		return PassThrough(&TypeInfo::AssignUInt, obj, value);
	}

	virtual bool AssignInt(byte* obj, int value) const override
	{
		return PassThrough(&TypeInfo::AssignInt, obj, value);
	}

	virtual bool AssignFloat(byte* obj, float value) const override
	{
		return PassThrough(&TypeInfo::AssignFloat, obj, value);
	}

	virtual bool AssignString(byte* obj, std::string value) const override
	{
		return PassThrough(&TypeInfo::AssignString, obj, value);
	}

	virtual ErrorOr<ReflectionObject> GetAtKey(
		byte* obj,
		HString name) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		byte* temp = reinterpret_cast<byte*>(&(*values)[t]);
		return typeInfo->GetAtKey(temp, name);
	}

	virtual bool InsertKey(byte* obj, HString name) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		byte* temp = reinterpret_cast<byte*>(&(*values)[t]);
		return typeInfo->InsertKey(temp, name);
	}

	virtual bool ObjectEnd(byte* obj) const override
	{
		return PassThrough(&TypeInfo::ObjectEnd, obj);
	}

	virtual ErrorOr<ReflectionObject> GetAtIndex(
		byte* obj,
		int index) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		byte* temp = reinterpret_cast<byte*>(&(*values)[t]);
		return typeInfo->GetAtIndex(temp, index);
	}

	virtual bool PushBackDefault(byte* obj) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		byte* temp = reinterpret_cast<byte*>(&(*values)[t]);
		return typeInfo->PushBackDefault(temp);
	}

	virtual bool ArrayEnd(byte* obj) const override
	{
		return PassThrough(&TypeInfo::ArrayEnd, obj);
	}
};

template<typename T>
struct TypeInfoStruct;

template<typename T>
struct MemberInfo
{
	friend struct TypeInfoStruct<T>;
protected:
	HString name;
	TypeInfo* typeInfo;

	MemberInfo(HString name, TypeInfo* typeInfo)
		: name(name)
		, typeInfo(typeInfo)
	{ }

public:
	virtual ~MemberInfo() { }

	virtual ReflectionObject Get(T* t) const = 0;
};

template<typename T, typename TMem>
struct MemberInfoTyped : public MemberInfo<T>
{
protected:
	TMem T::* location;

public:
	MemberInfoTyped(HString name, TMem T::* location, TypeInfo* typeInfo = GetTypeInfo<TMem>())
		: MemberInfo<T>(name, typeInfo)
		, location(location)
	{ }

	virtual ReflectionObject Get(T* t) const override
	{
		return ReflectionObject(
			reinterpret_cast<byte*>(&(t->*location)),
			this->typeInfo);
	}
};

template<typename T, typename TMem>
MemberInfoTyped<T, TMem>* MakeMemberInfoTyped(HString name, TMem T::* location, TypeInfo* typeInfo = GetTypeInfo<TMem>())
{
	return new MemberInfoTyped<T, TMem>(name, location, typeInfo);
}

template<typename T>
struct TypeInfoStruct : public TypeInfo
{
	// rmf note: at first all TypeInfo would have the opportunity for a parent type
	// but I figured it was mostly unecessary for the others. Feel free to change in the future.
	TypeInfo* parentType;
	std::vector<MemberInfo<T>*> vMembers;
	bool(*pPostLoad)(T& object);

	TypeInfoStruct(
		HString name,
		TypeInfo* parentType,
		std::vector<MemberInfo<T>*> members,
		bool(*pPostLoad)(T& object) = nullptr)
		: TypeInfo(name)
		, parentType(parentType)
		, vMembers(members)
		, pPostLoad(pPostLoad)
	{}

	~TypeInfoStruct()
	{
		for (auto pMember : vMembers)
		{
			delete pMember;
		}
	}

	virtual ErrorOr<ReflectionObject> GetAtIndex(
		byte* obj,
		int index) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		if (vMembers.size() <= index)
		{
			return Error(
				name
				+ " struct GetAtIndex "
				+ std::to_string(index)
				+ " is out of bounds, "
				+ std::to_string(vMembers.size())
				+ ".");
		}
		return vMembers[index]->Get(t);
	}

	virtual bool PushBackDefault(byte* obj) const override
	{
		// has no meaning, failure would occur in GetAtIndex if array is too long
		return true;
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
				return member->Get(t);
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

	// implemented as a no-op to unify table and struct deserialization
	virtual bool InsertKey(byte* obj, HString name) const override
	{
		return !(GetAtKey(obj, name).IsError());
	}

	virtual bool ObjectEnd(byte* obj) const override
	{
		if (pPostLoad == nullptr) return true;
		T* t = reinterpret_cast<T*>(obj);
		return pPostLoad(*t);
	}
};

} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_DEFINE_H
