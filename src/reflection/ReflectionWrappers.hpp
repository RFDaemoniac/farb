#ifndef FARB_REFLECTION_WRAPPERS_H
#define FARB_REFLECTION_WRAPPERS_H

#include "ReflectionDeclare.h"
#include "ReflectionDefine.hpp"
#include "NamedType.hpp"
#include "ValueCheckedType.hpp"
#include "BuiltinTypedefs.h"

namespace Farb
{

namespace Reflection
{


template<typename T>
struct TemplatedTypeInfo<value_ptr<T> >
{
	struct Converter
	{
		virtual ErrorOr<value_ptr<T> > operator()(const T & in) override
		{
			return value_ptr<T>(in);
		}
	};

	static TypeInfo* Get()
	{
		static Converter converter;
		static TypeInfoAs<value_ptr<T>, T> typeInfo{
			"value_ptr<" + GetTypeInfo<T>()->GetName() + ">",
			converter};
		return &typeInfo;
	}
};

template<typename T, typename Tag>
struct TemplatedTypeInfo<NamedType<T, Tag>>
{
	// rmf todo: @implement ErrorOr for all of the api
	template<typename TVal>
	static bool Assign(NamedType<T, Tag>& obj, TVal value)
	{
		auto reflect = Reflect(obj.value);
		auto pTypeInfo = dynamic_cast<TypeInfoCustomLeaf<T>*>(reflect.typeInfo);
		if (pTypeInfo == nullptr)
		{
			return false;
		}
		return pTypeInfo->Assign(reflect.location, value);
	}

	static std::string ToString(const NamedType<T, Tag>& object)
	{
		return Reflection::ToString(object.value);
	}

	static TypeInfo* Get()
	{
		static auto namedTypeInfo = TypeInfoCustomLeaf<NamedType<T, Tag> >::Construct(
			Tag::GetName(),
			ToString,
			Assign<bool>,
			Assign<uint>,
			Assign<int>,
			Assign<float>,
			Assign<std::string>);

		return &namedTypeInfo;
	}
};

template<typename T, typename Tag>
struct TemplatedTypeInfo<ValueCheckedType<T, Tag> >
{
	template<typename TVal>
	static bool Assign(ValueCheckedType<T, Tag>& obj, TVal value)
	{
		auto pTypeInfo = GetTypeInfo<T>();
		auto pTypeInfoLeaf = dynamic_cast<TypeInfoCustomLeaf<T>*>(pTypeInfo);
		if (pTypeInfoLeaf == nullptr)
		{
			return false;
		}
		T unchecked{};
		auto uncheckedReflect = Reflect(unchecked);
		if (!pTypeInfoLeaf->Assign(uncheckedReflect.location, value))
		{
			return false;
		}
		auto result = ValueCheckedType<T, Tag>::TryCreate(unchecked);
		if (result.IsError())
		{
			return false;
		}
		obj = result.GetValue();
		return true;
	}

	static std::string ToString(const ValueCheckedType<T, Tag>& object)
	{
		return Reflection::ToString(object.GetValue());
	}

	static TypeInfo* Get()
	{
		static auto namedTypeInfo = TypeInfoCustomLeaf<ValueCheckedType<T, Tag> >::Construct(
			Tag::GetName(),
			ToString,
			Assign<bool>,
			Assign<uint>,
			Assign<int>,
			Assign<float>,
			Assign<std::string>);

		return &namedTypeInfo;
	}
};

} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_WRAPPERS_H
