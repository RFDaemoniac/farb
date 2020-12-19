#ifndef FARB_VALUE_CHECKED_TYPE_HPP
#define FARB_VALUE_CHECKED_TYPE_HPP

#include <type_traits>

#include "../reflection/ReflectionDeclare.h"
#include "ErrorOr.hpp"
#include "../utils/TypeInspection.hpp"

namespace Farb
{

/*
struct ValueCheckedTypeTag
{
	static bool IsValid(const T& value);

	static HString GetName();
};
*/

template<typename T, typename Tag>
struct ValueCheckedType
{
	static_assert(
		std::is_same<decltype(Tag::IsValid), bool (const T&)>::value,
		"ValueCheckedType Tag must have function static Bool IsValid(const T& value)");
	static_assert(
		std::is_same<decltype(Tag::GetName), HString ()>::value,
		"ValueCheckedType Tag must have function static HString GetName");

private:	
	T value;

public:
	bool PostDeserialize()
	{
		return Tag::IsValid(value);
	}

	static ErrorOr<ValueCheckedType> TryCreate(const T& value)
	{
		if (!Tag::IsValid(value))
		{
			return Error("Invalid Value: " + Reflection::ToString(value));
		}
		return ValueCheckedType(value);
	}

	// rmf todo: ideally this would be removed
	ValueCheckedType()
		: value()
	{ }

	ValueCheckedType(const ValueCheckedType& other)
		: value(other.value)
	{ }

	const T& GetValue() const { return value; }

protected:
	ValueCheckedType(const T& value)
		: value(value)
	{ }
};

} // namespace Farb

#endif // FARB_VALUE_CHECKED_TYPE_HPP