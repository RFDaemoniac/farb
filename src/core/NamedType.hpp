#ifndef FARB_NAMED_TYPE_HPP
#define FARB_NAMED_TYPE_HPP

#include "BuiltinTypedefs.h"
#include "../utils/TypeInspection.hpp"

namespace Farb
{

#define NAMED_TYPE_TAG(NAME) struct NAME ## Tag { static HString GetName() { return #NAME; } }

#define NAMED_TYPE_USING(TYPE, NAME) using NAME = NamedType<TYPE, NAME ## Tag>

#define NAMED_TYPE(TYPE, NAME) NAMED_TYPE_TAG(NAME); NAMED_TYPE_USING(TYPE, NAME);

/*
struct NamedTypeTag
{
	static HString GetName();
};
*/

template<typename T, typename Tag>
struct NamedType
{
	/* rmf todo: implement this, why isn't it working?
	static_assert(
		std::is_same<decltype(Tag::GetName), HString(*)()>::value,
		"NamedType Tag must have function static HString GetName");
	*/
	T value;

	// rmf todo: should default constructor be public?
	// I'd rather not but I don't really see a way around it
	// rmf note: this is not to be used except by objects that will be reflected
	NamedType()
		: value()
	{ }

	NamedType(const T& value)
		: value(value)
	{ }

	// ugly sfinae to allow implicit construction from string literals
	template<typename U = T>
	NamedType(const char* value, typename std::enable_if<std::is_same<U, std::string>::value >::type* = nullptr)
		: value(value)
	{ }

	NamedType(const NamedType& other)
		: value(other.value)
	{ }

	bool operator ==(const NamedType& other) const
	{
		return value == other.value;
	}

	bool operator !=(const NamedType& other) const
	{
		return value != other.value;
	}

	bool operator <(const NamedType& other) const
	{
		return value < other.value;
	}

	bool operator >(const NamedType& other) const
	{
		return value > other.value;
	}
};

} // namespace Farb

namespace std
{
	template <typename T, typename Tag>
	struct hash<Farb::NamedType<T, Tag> >
	{
		std::size_t operator()(const Farb::NamedType<T, Tag>& namedValue) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return hash<T>()(namedValue.value);
		}
	};
} // namespace std

#endif // FARB_NAMED_TYPE_HPP