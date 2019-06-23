#ifndef FARB_REFLECTION_HPP
#define FARB_REFLECTION_HPP

#include "../../lib/visit_struct/visit_struct.hpp"
#include "../hstring/hstring.h"

#define REFLECT_STRUCT(...) VISITABLE_STRUCT(__VA_ARGS__)

namespace farb
{

namespace reflection
{

namespace SetMemberResult
{
	enum Enum
	{
		Success = 0,
		TypeMismatch = 1,
		MemberNameNotFound = 2,
	}
}

struct SetMemberReturn
{
	SetMemberResult::Enum result;
	std::size_t lastVisitedFieldIndex;
}

// lastVisitedIndex can be passed in to optimize for case where
// deserializing from an object defined in order
template <typename S, typename V>
SetMemberReturn SetMember(S& s, HString memberName, const V& value, std::size_t lastVisitedIndex = visit_struct::field_count<S>())
{
	std::size_t fieldCount = visit_struct::field_count(s);
	std:: size_t i = lastVisitedFieldIndex;
	set_field_return ret {false, i};
	bool success = false;
	do
	{
		i = (i + 1) % fieldCount;
		if (visit_struct::get_name<i>(s) == memberName)
		{
			if (std::is_same<visit_struct::type_at<i, S>, visit_struct::type_of<V> >::value)
			{
				ret.result = SetMemberResult::Success;
				visit_struct::get<i>(s) = value;
			}
			else
			{
				ret.result = SetMemberResult::TypeMismatch;
			}
			break;
		}
	}
	while (i != lastVisitedFieldIndex);

	ret.lastVisitedFieldIndex = i;
	if (ret.result != SetMemberResult::Sucess
		&& ret.lastVisitedIndex == lastVisitedIndex)
	{
		ret.result = MemberNameNotFound;
	}
	return success;

	for (std:: size_t i = lastVisitedFieldIndex + 1 % field_count;
		i != lastVisitedFieldIndex; i = i + 1 % field_count)
	std::size_t i = lastVisitedFieldIndex + 1 % field_count;
	visit_struct::get<i>(s) = value;
}

} // namespace reflection

} // namespace farb


#endif // FARB_REFLECTION_HPP