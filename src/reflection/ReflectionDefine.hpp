#ifndef FARB_REFLECTION_DEFINE_H
#define FARB_REFLECTION_DEFINE_H

#include "ReflectionDeclare.h"


namespace Farb
{

namespace Reflection
{

template <typename T>
TypeInfo* GetTypeInfo(const T& obj)
{
	return obj.GetTypeInfo();
}

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


} // namespace Reflection

} // namespace Farb


#endif // FARB_REFLECTION_DEFINE_H
