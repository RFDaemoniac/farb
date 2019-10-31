#ifndef FARB_TYPE_INSPECTION_HPP
#define FARB_TYPE_INSPECTION_HPP

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
template < typename... >
struct TypeList {};

template<typename A, typename B>
struct TypeListUnion;

template<typename... As, typename... Bs>
struct TypeListUnion<TypeList<As...>, TypeList<Bs...> >
{
	using Types = TypeList<As..., Bs...>;
};

template<typename TNeedle, typename THay, typename...THaystack>
struct SplitTypeList
{
	using Next = SplitTypeList<TNeedle, THaystack...>;

	using Before = typename TypeListUnion<
		TypeList<THay>,
		typename Next::Before>::Types;
	using After = typename Next::After;
};

template<typename TNeedle, typename...THaystack>
struct SplitTypeList<TNeedle, TNeedle, THaystack...>
{
	using Before = TypeList<>;
	using After = TypeList<THaystack...>;
};

} // namespace Farb

#endif // FARB_TYPE_INSPECTION_HPP
