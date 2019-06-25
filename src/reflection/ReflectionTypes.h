// based on http://donw.io/post/reflection-cpp-1/
#ifndef FARB_REFLECTION_TYPES_H
#define FARB_REFLECTION_TYPES_H

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

template<typename T>
struct TypeInfoCustomLeaf : public TypeInfo
{
protected:
	bool (T::*fAssignBool)(bool);
	bool (T::*fAssignUInt)(uint);
	bool (T::*fAssignInt)(int);
	bool (T::*fAssignFloat)(float);
	bool (T::*fAssignString)(std::string);

public:
	TypeInfoCustomLeaf()
		: fAssignBool(nullptr)
		, fAssignUInt(nullptr)
		, fAssignInt(nullptr)
		, fAssignFloat(nullptr)
		, fAssignString(nullptr)
	{
	}

	virtual bool AssignBool(byte* obj, bool value) const override
	{
		if (fAssignBool == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*fAssignBool)(value);
	}

	virtual bool AssignUInt(byte* obj, uint value) const override
	{
		if (fAssignUInt == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*fAssignUInt)(value);
	}

	virtual bool AssignInt(byte* obj, int value) const override
	{
		if (fAssignInt == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*fAssignInt)(value);
	}

	virtual bool AssignFloat(byte* obj, float value) const override
	{
		if (fAssignFloat == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*fAssignFloat)(value);
	}

	virtual bool AssignString(byte* obj, std::string value) const override
	{
		if (fAssignString == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*fAssignString)(value);
	}
};

struct ReflectionContext
{
	ReflectionContext* parentContext;
	TypeInfo* typeInfo;
	byte* object;
};

template<typename T>
struct TypeInfoEnum : public TypeInfo
{
protected:
	std::vector<std::pair<HString, int> > vValues;

public:

	TypeInfoEnum(HString name, TypeInfo* parent, std::vector<std::pair<HString, int> > vEnumValues)
		: TypeInfo(name, parent)
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

template<typename T, typename TVal>
struct TypeInfoArray : public TypeInfo
{
	// rmf todo: @implement
};

template<typename T, typename TKey, typename TVal>
struct TypeInfoTable : public TypeInfo
{
	// rmf todo: @implement
};

struct TypeInfoStruct : public TypeInfo
{
	struct MemberInfo
	{
		HString name;
		uint offset;
		TypeInfo* typeInfo;
	};

	std::vector<MemberInfo> vMembers;

	virtual bool GetAtKey(
		byte* obj,
		HString name,
		TypeInfo* outInfo,
		byte* outObj) const
	{
		for (const auto & member : vMembers)
		{
			if (member.name == name)
			{
				outObj = obj + member.offset;
				outInfo = member.typeInfo;
				return true;
			}
		}
		return false;
	}
};

// should this really be handled like this?
struct ReflectableObject
{
	virtual TypeInfo* GetTypeInfo() const;
};

struct ExampleReflectableObject : public ReflectableObject
{
	static TypeInfo* typeInfo;
	virtual TypeInfo* GetTypeInfo() const override { return typeInfo; }
};


} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_TYPES_H