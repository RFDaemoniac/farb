#ifndef FARB_REFLECTION_DECLARE_H
#define FARB_REFLECTION_DECLARE_H

#include <string>
#include <type_traits>
#include <vector>

namespace Farb
{

// rmf todo: @implement HString or another pooled string option
// ideally one that allows for a constexpr constructor
using HString = std::string;
using uint = unsigned int;
using byte = unsigned char;

namespace Reflection
{

struct TypeInfo;

struct ReflectionContext
{
	ReflectionContext* parentContext;
	TypeInfo* typeInfo;
	byte* object;
};

struct TypeInfo
{
protected:
	HString name;

public:
	TypeInfo(HString name)
		: name(name)
	{

	}

	virtual bool AssignBool(byte* obj, bool value) const { return false; }
	virtual bool AssignUInt(byte* obj, uint value) const { return false; }
	virtual bool AssignInt(byte* obj, int value) const { return false; }
	virtual bool AssignFloat(byte* obj, float value) const { return false; }
	virtual bool AssignString(byte* obj, std::string value) const { return false; }


	struct GetAtResult
	{
		bool success;
		TypeInfo* typeInfo;
		byte* obj;

		GetAtResult()
			: success(false)
			, typeInfo(nullptr)
			, obj(nullptr)
		{ }

		GetAtResult(TypeInfo* typeInfo, byte* obj)
			: success(true)
			, typeInfo(typeInfo)
			, obj(obj)
		{ }
	};

	virtual GetAtResult GetAtKey(
		byte* obj,
		HString name) const
	{ return GetAtResult(); }

	virtual bool InsertKey(byte* obj, HString name) const { return false; }

	// if index is < length, returns an already existing value
	// if index is > length, fails
	virtual GetAtResult GetAtIndex(
		byte* obj,
		int index) const
	{ return GetAtResult(); }

	virtual bool PushBackDefault(byte* obj) const { return false; }
};

template <typename T>
TypeInfo* GetTypeInfo()
{
	return &T::typeInfo;
}

// helper for defining something when template arguments resolve correctly
// and therefore also being able to use sfinae when they don't
template<typename...> struct voider { using type = void; };
template<typename... Ts> using void_t = typename voider<Ts...>::type;

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


template<typename T, typename std::enable_if<has_GetTypeInfo<T>{}, int>::type = 0>
TypeInfo* GetTypeInfo(const T& obj)
{
	return obj.GetTypeInfo();
}

template<typename T, typename std::enable_if<!has_GetTypeInfo<T>{}, int>::type = 0>
TypeInfo* GetTypeInfo(const T& obj)
{
	return GetTypeInfo<T>();
}


} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_DECLARE_H
