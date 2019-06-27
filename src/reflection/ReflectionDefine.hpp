#ifndef FARB_REFLECTION_DEFINE_H
#define FARB_REFLECTION_DEFINE_H

#include <memory>

#include "ReflectionDeclare.h"


namespace Farb
{

namespace Reflection
{

template<typename T>
struct TypeInfoCustomLeaf : public TypeInfo
{
protected:
	bool (T::*pAssignBool)(bool);
	bool (T::*pAssignUInt)(uint);
	bool (T::*pAssignInt)(int);
	bool (T::*pAssignFloat)(float);
	bool (T::*pAssignString)(std::string);

public:
	TypeInfoCustomLeaf()
		: pAssignBool(nullptr)
		, pAssignUInt(nullptr)
		, pAssignInt(nullptr)
		, pAssignFloat(nullptr)
		, pAssignString(nullptr)
	{
	}

	virtual bool AssignBool(byte* obj, bool value) const override
	{
		if (pAssignBool == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*pAssignBool)(value);
	}

	virtual bool AssignUInt(byte* obj, uint value) const override
	{
		if (pAssignUInt == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*pAssignUInt)(value);
	}

	virtual bool AssignInt(byte* obj, int value) const override
	{
		if (pAssignInt == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*pAssignInt)(value);
	}

	virtual bool AssignFloat(byte* obj, float value) const override
	{
		if (pAssignFloat == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*pAssignFloat)(value);
	}

	virtual bool AssignString(byte* obj, std::string value) const override
	{
		if (pAssignString == nullptr) { return false; }
		T* t = reinterpret_cast<T*>(obj);
		return (t->*pAssignString)(value);
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
	virtual bool GetAtIndex(
		byte* obj,
		int index,
		TypeInfo* outInfo,
		byte* outObj) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		if (t->size() <= index)
		{
			return false;
		}
		TVal* tval = reinterpret_cast<T*>(&t[index]);
		outObj = tval;
		outInfo = GetTypeInfo(*tval);
		return true;
	}

	virtual bool PushBackDefault(byte* obj) const override
	{
		T* t = reinterpret_cast<T*>(obj);
		t->push_back(TVal());
	}
};

template<typename T, typename TKey, typename TVal>
struct TypeInfoTable : public TypeInfo
{
	// rmf todo: @implement
};

template<typename T>
struct MemberInfo
{
	HString name;
	TypeInfo* typeInfo;

	MemberInfo(HString name, TypeInfo* typeInfo)
		: name(name)
		, typeInfo(typeInfo)
	{}

	virtual ~MemberInfo() {}

	virtual byte* GetLocation(T* t) const = 0;
};

template<typename T, typename TMem>
struct MemberInfoTyped : public MemberInfo<T>
{
	TMem T::* location;

	MemberInfoTyped(HString name, TMem T::* location)
		: MemberInfo<T>(name, GetTypeInfo<TMem>())
		, location(location)
	{}

	virtual byte* GetLocation(T* t) const override
	{
		return reinterpret_cast<byte*>(&(t->*location));
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

	virtual bool GetAtKey(
		byte* obj,
		HString name,
		TypeInfo* outInfo,
		byte* outObj) const
	{
		T* t = reinterpret_cast<T*>(obj);
		for (auto member : vMembers)
		{
			if (member->name == name)
			{
				outObj = member->GetLocation(t);
				outInfo = member->typeInfo;
				return true;
			}
		}

		if (parentType != nullptr)
		{
			return parentType->GetAtKey(obj, name, outInfo, outObj);
		}
		return false;
	}
};


} // namespace Reflection

} // namespace Farb


#endif // FARB_REFLECTION_DEFINE_H
