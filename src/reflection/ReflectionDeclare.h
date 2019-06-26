// based on http://donw.io/post/reflection-cpp-1/
#ifndef FARB_REFLECTION_DECLARE_H
#define FARB_REFLECTION_DECLARE_H

#include <string>
#include <vector>

namespace Farb
{

using HString = std::string;
using uint = unsigned int;
using byte = unsigned char;

namespace Reflection
{

struct TypeInfo
{
protected:
	HString name;
	TypeInfo* parentType;

public:
	TypeInfo(HString name, TypeInfo* parentType)
		: name(name)
		, parentType(parentType)
	{

	}

	virtual bool AssignBool(byte* obj, bool value) const { return false; }
	virtual bool AssignUInt(byte* obj, uint value) const { return false; }
	virtual bool AssignInt(byte* obj, int value) const { return false; }
	virtual bool AssignFloat(byte* obj, float value) const { return false; }
	virtual bool AssignString(byte* obj, std::string value) const { return false; }

	virtual bool GetAtKey(
		byte* obj,
		HString name,
		TypeInfo* outInfo,
		byte* outObj) const
	{ return false; }

	virtual bool InsertKey(byte* obj, HString name) const { return false; }

	// if index is < length, returns an already existing value
	// if index is > length, fails
	virtual bool GetAtIndex(
		byte* obj,
		int index,
		TypeInfo* outInfo,
		byte* outObj) const
	{ return false; }

	virtual bool PushBackDefault(byte* obj) const { return false; }
};

template <typename T>
TypeInfo* GetTypeInfo(const T& obj)
{
	return obj.GetTypeInfo();
}

template <typename T>
TypeInfo* GetTypeInfo()
{
	return &T::typeInfo;
}

struct ReflectionContext
{
	ReflectionContext* parentContext;
	TypeInfo* typeInfo;
	byte* object;
};


} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_DECLARE_H
