#ifndef FARB_REFLECTION_DECLARE_H
#define FARB_REFLECTION_DECLARE_H

#include <string>
#include <type_traits>
#include <vector>
#include <experimental/type_traits>

#include "../core/BuiltinTypedefs.h"
#include "../core/ErrorOr.hpp"

namespace Farb
{

namespace Reflection
{

// this is not a strict limit
// we will wrap after reaching 100 characters and finishing the current serialization
constexpr int SerializationLineLengthTarget = 100;

struct TypeInfo;

struct ReflectionObject
{
	// todo: add per-reflection tracking like required members, etc

	byte* location;
	TypeInfo* typeInfo;

	ReflectionObject()
		: location(nullptr)
		, typeInfo(nullptr)
	{ }

	ReflectionObject(byte* location, TypeInfo* typeInfo)
		: location(location)
		, typeInfo(typeInfo)
	{ }

	ReflectionObject(const ReflectionObject& other)
		: location(other.location)
		, typeInfo(other.typeInfo)
	{ }

	// rmf note: should these be const if they modify the values?
	// even though it doesn't change the direct member
	// it does change the value at location
	// if it were a reference they wouldn't be const
	bool AssignBool(bool value) const;
	bool AssignUInt(uint value) const;
	bool AssignInt(int value) const;
	bool AssignFloat(float value) const;
	bool AssignString(std::string value) const;

	ErrorOr<ReflectionObject> GetAtKey(HString name) const;
	bool InsertKey(HString name) const;
	bool ObjectEnd() const;

	ErrorOr<ReflectionObject> GetAtIndex(int index) const;
	bool PushBackDefault() const;
	bool ArrayEnd() const;

	ErrorOr<std::string> ToString() const;
};

struct TypeInfo
{	
	friend struct ReflectionObject;
protected:
	HString name;

public:
	TypeInfo(HString name)
		: name(name)
	{ }

	const HString& GetName() const { return name; }

	virtual bool AssignBool(byte* obj, bool value) const { return false; }
	virtual bool AssignUInt(byte* obj, uint value) const { return false; }
	virtual bool AssignInt(byte* obj, int value) const { return false; }
	virtual bool AssignFloat(byte* obj, float value) const { return false; }
	virtual bool AssignString(byte* obj, std::string value) const { return false; }

	virtual ErrorOr<ReflectionObject> GetAtKey(
		byte* obj,
		HString name) const
	{
		return Error(name + " GetAtKey not impelmented.");
	}

	virtual bool InsertKey(byte* obj, HString name) const { return false; }

	virtual ErrorOr<ReflectionObject> GetAtIndex(
		byte* obj,
		int index) const
	{
		return Error(name + " GetAtIndex not impelmented.");
	}
	// rmf todo: why did I want this one to be true?
	virtual bool ObjectEnd(byte* obj) const { return true; }

	virtual bool PushBackDefault(byte* obj) const { return false; }
	virtual bool ArrayEnd(byte* obj) const { return false; }

	virtual ErrorOr<std::string> ToString(byte* obj) const
	{
		return Error(name + " ToString not implemented");
	}
};

/*
// helper for defining something when template arguments resolve correctly
// and therefore also being able to use sfinae when they don't


template<typename T, typename = void>
struct has_GetTypeInfo : std::false_type {};

// this resolution is preferred to the above because it specifies the second template type
// rather than relying on the default argument for the second template type
// and it's only possible if decltype(T::GetTypeInfo) is valid
// otherwise fails, but sfinae, so we fall back on the above
// partial specialization only works on structs, not functions, otherwise we could apply this
// directly in the function definition
template<typename T>
struct has_GetTypeInfo<T, void_t<decltype(T::GetTypeInfo)> > : std::true_type {};
*/

template<typename...> struct voider { using type = void; };
template<typename... Ts> using void_t = typename voider<Ts...>::type;

template <typename T>
struct TemplatedTypeInfo
{
	static TypeInfo* Get();
};

//template <class T>
//using has_GetStaticTypeInfo = decltype(T::typeInfo);

template <class T, typename = void>
struct has_GetStaticTypeInfo : std::false_type {};

template<typename T>
struct has_GetStaticTypeInfo<T, void_t<decltype(T::GetStaticTypeInfo)> > : std::true_type {};

template <typename T>
TypeInfo* GetTypeInfo()
{
	//if constexpr (std::experimental::is_detected<has_GetStaticTypeInfo, T>::value)
	if constexpr (has_GetStaticTypeInfo<T>::value)
	{
		return T::GetStaticTypeInfo();
	}
	else
	{
		return TemplatedTypeInfo<T>::Get();
	}
}

template <class T, typename = void>
struct has_GetInstanceTypeInfo : std::false_type {};

template<typename T>
struct has_GetInstanceTypeInfo<T, void_t<decltype(std::declval<T>().GetInstanceTypeInfo())> > : std::true_type {};


template<typename T>
inline TypeInfo* GetTypeInfo(const T& obj)
{
	if constexpr (has_GetInstanceTypeInfo<T>::value)
	{
		return obj.GetInstanceTypeInfo();
	}
	else
	{
		return GetTypeInfo<T>();
	}
}

template<typename T>
inline ReflectionObject Reflect(T& object)
{
	return ReflectionObject(
		reinterpret_cast<byte*>(&object),
		GetTypeInfo(object));
}

template<typename T>
inline std::string ToString(const T& obj)
{
	// casting away const should be fine here because
	// ToString and GetName both do not modify the underlying value
	auto reflect = Reflect(const_cast<T&>(obj));
	auto result = reflect.ToString();
	if (result.IsError())
	{
		// rmf todo: log error
		return "Uknown value of type " + reflect.typeInfo->GetName();
	}
	return result.GetValue();
}

inline bool ReflectionObject::AssignBool(bool value) const
{
	return typeInfo->AssignBool(location, value);
}

inline bool ReflectionObject::AssignUInt(uint value) const
{
	return typeInfo->AssignUInt(location, value);
}

inline bool ReflectionObject::AssignInt(int value) const
{
	return typeInfo->AssignInt(location, value);
}

inline bool ReflectionObject::AssignFloat(float value) const
{
	return typeInfo->AssignFloat(location, value);
}

inline bool ReflectionObject::AssignString(std::string value) const
{
	return typeInfo->AssignString(location, value);
}

inline ErrorOr<ReflectionObject> ReflectionObject::GetAtKey(HString name) const
{
	return typeInfo->GetAtKey(location, name);
}

inline bool ReflectionObject::InsertKey(HString name) const
{
	return typeInfo->InsertKey(location, name);
}

inline ErrorOr<ReflectionObject> ReflectionObject::GetAtIndex(int index) const
{
	return typeInfo->GetAtIndex(location, index);
}

inline bool ReflectionObject::ObjectEnd() const
{
	 return typeInfo->ObjectEnd(location);
}

inline bool ReflectionObject::PushBackDefault() const
{
	return typeInfo->PushBackDefault(location);
}

inline bool ReflectionObject::ArrayEnd() const
{
	return typeInfo->ArrayEnd(location);
}

inline ErrorOr<std::string> ReflectionObject::ToString() const
{
	return typeInfo->ToString(location);
}

} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_DECLARE_H
