#ifndef FARB_VALUE_CHECKED_TYPE_HPP
#define FARB_VALUE_CHECKED_TYPE_HPP

#include <type_traits>

#include "ErrorOr.hpp"
#include "../utils/TypeInspection.hpp"

namespace Farb
{

/*
struct ValueCheckedTypeTag
{
	static Bool IsValid(const T& value);

	static HString GetName();
};
*/

template<typename T, typename Tag>
struct ValueCheckedType
{
	/* rmf todo: why doesn't this work?
	static_assert(
		std::is_same<decltype(Tag::IsValid), bool(*)(const T&)>::value,
		"ValueCheckedType Tag must have function static Bool IsValid(const T& value)");
	static_assert(
		std::is_same<decltype(Tag::GetName), HString(*)()>::value,
		"ValueCheckedType Tag must have function static HString GetName");
	*/

private:	
	T value;

public:
	bool PostDeserialize()
	{
		return Tag::IsValid(value);
	}

	static ErrorOr<ValueCheckedType> TryCreate(T value)
	{
		if (!Tag::IsValid(value))
		{
			return Error("Invalid Value: " + ToString(value));
		}
		return new ValueCheckedType(value);
	}

	// rmf todo: ideally this would be removed
	ValueCheckedType()
		: value()
	{ }

	ValueCheckedType(const T& value)
		: value(value)
	{ }

	ValueCheckedType(const ValueCheckedType& other)
		: value(other.value)
	{ }

	const T& GetValue() { return value; }
};

} // namespace Farb

#endif // FARB_VALUE_CHECKED_TYPE_HPP