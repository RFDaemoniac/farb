#ifndef FARB_TYPE_INSPECTION_HPP
#define FARB_TYPE_INSPECTION_HPP

#include "ErrorOr.hpp"

namespace Farb
{

// rmf todo: these were built under a specific use case for reflection
// might want to do some work to generalize them

template<typename T>
struct ExtractFunctionTypes
{ };

template<typename C, typename R, typename A>
struct ExtractFunctionTypes<R(C::*)(A)>
{
	using IsMember = std::true_type;
	using Class = C;
	using Return = R;
	using Param = A;
};

template<typename C, typename R, typename A>
struct ExtractFunctionTypes<R(*)(C&, A)>
{
	using IsMember = std::false_type;
	using Class = C;
	using Return = R;
	using Param = A;
};

template<typename C, typename R>
struct ExtractFunctionTypes<R(C::*)()>
{
	using IsMember = std::true_type;
	using Class = C;
	using Return = R;
};

template<typename C, typename R>
struct ExtractFunctionTypes<R(*)(C&)>
{
	using IsMember = std::false_type;
	using Class = C;
	using Return = R;
};

// A template to hold a parameter pack
template<typename... Ts>
struct TypeList
{
	static constexpr std::size_t Count = sizeof...(Ts);
};

template<std::size_t NTarget, typename T, typename... Ts>
struct GetNth
{
	using Type = typename GetNth<NTarget - 1, Ts...>::Type;
};

template<typename T, typename... Ts>
struct GetNth<0, T, Ts...>
{
	using Type = T;
};

template <std::size_t N, typename TList>
struct Nested_GetNth;

template <std::size_t N, template<class...> class TContainer, class... Ts>
struct Nested_GetNth<N, TContainer<Ts...> >
{
	using Type = GetNth<N, Ts...>;
};

/*
template<typename A, typename B>
struct TypeListUnion;

template<typename... As, typename... Bs>
struct TypeListUnion<TypeList<As...>, TypeList<Bs...> >
{
	using Types = TypeList<As..., Bs...>;
};
*/

template<typename... TLists>
struct TypeListUnion;

template<typename... Ts>
struct TypeListUnion<TypeList<Ts...> >
{
	using Type = TypeList<Ts...>;
};

template<typename ... TAs, typename... TBs, typename ... TListsRemaining>
struct TypeListUnion<TypeList<TAs...>, TypeList<TBs...>, TListsRemaining...>
{
	using Type =
		typename TypeListUnion<
			TypeList<TAs..., TBs...>,
			TListsRemaining...
		>::Type;
};

template<typename TNeedle, typename THaystack>
struct SplitTypeList;

template<typename TNeedle, typename THay, typename...THaystack>
struct SplitTypeList<TNeedle, TypeList<THay, THaystack...> >
{
	using Next = SplitTypeList<TNeedle, TypeList<THaystack...> >;

	using Before = typename TypeListUnion<
		TypeList<THay>,
		typename Next::Before>::Type;
	using After = typename Next::After;
};

template<typename TNeedle, typename...THaystack>
struct SplitTypeList<TNeedle, TypeList<TNeedle, THaystack...> >
{
	using Before = TypeList<>;
	using After = TypeList<THaystack...>;
};

template<typename TNeedle, typename THay, typename...THaystack>
struct RemoveTypeFromTypeList
{
	using Next = RemoveTypeFromTypeList<TNeedle, THaystack...>;

	using Remaining = typename TypeListUnion<
		TypeList<THay>,
		typename Next::Remaining>::Type;
};

template<typename TNeedle, typename...THaystack>
struct RemoveTypeFromTypeList<TNeedle, TNeedle, THaystack...>
{
	using Remaining = TypeList<THaystack...>;
};

// SFINAE type inspection
template<typename TVal>
struct IsErrorOr : std::false_type { };

template<typename TVal>
struct IsErrorOr<ErrorOr<TVal> > : std::true_type { };

template<typename T>
struct UnwrapErrorOr
{
	using TVal = T;
};

template<typename T>
struct UnwrapErrorOr<ErrorOr<T> >
{
	using TVal = T;
};

template<typename T>
T ValueOrDefault(ErrorOr<T> result, T default_value)
{
	if (result.IsError())
	{
		return default_value;
	}
	return result.GetValue();
}

} // namespace Farb

#endif // FARB_TYPE_INSPECTION_HPP
