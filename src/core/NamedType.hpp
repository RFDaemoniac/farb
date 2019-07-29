#ifndef FARB_NAMED_TYPE_HPP
#define FARB_NAMED_TYPE_HPP

#include "BuiltinTypedefs.h"
#include "../utils/TypeInspection.hpp"

namespace Farb
{

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

	NamedType(const NamedType& other)
		: value(other.value)
	{ }

	bool operator ==(const NamedType& other) const
	{
		return value == other.value;
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