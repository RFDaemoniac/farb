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
};

} // namespace Farb

#endif // FARB_NAMED_TYPE_HPP