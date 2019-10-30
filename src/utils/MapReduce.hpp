#ifndef FARB_MAP_REDUCE_HPP
#define FARB_MAP_REDUCE_HPP

#include "ErrorOr.hpp"

namespace Farb
{

template<typename TRet, typename ...TArgs>
struct Functor
{
	virtual TRet operator()(TArgs... args);
};

template<typename ...TArgs>
struct Functor<void, TArgs...>
{
	virtual void operator()(TArgs... args);
};

template<typename TRet, typename ...TBefore, typename TArg, typename ...TAfter>
struct CurriedFunctor : public Functor<TRet, TBefore..., TAfter...>
{
	value_ptr<Functor<TRet, TBefore..., TArg, TAfter...>> functor;
	TArg value;

	CurriedFunctor(Functor<TRet, TBefore..., TArg, TAfter...> functor, TArg value)
		: functor(functor)
		, value(value)
	{ }

	CurriedFunctor(CurriedFunctor&& other)
		: functor(other.functor)
		, value(other.value)
	{ }

	virtual TRet operator()(TBefore... before, TAfter... after) override
	{
		return functor(before..., value, after...);
	}
};

template<
	typename TRet,
	typename ...TBefore,
	typename TArg,
	typename ...TAfter,
	typename ...T2Args>
struct ComposedFunctors : public Functor <TRet, TBefore..., T2Args..., TAfter...>
{
	value_ptr<Functor<TRet, TBefore..., UnwrapErrorOr<TArg>::TVal, TAfter...> > functor;
	value_ptr<Functor<TArg, T2Args...> > functor_two;

	// by convention assume that functions don't use ErrorOr as parameters
	ComposedFunctor(
		Functor<TRet, TBefore..., UnwrapErrorOr<TArg>::TVal, TAfter...> functor,
		Functor<TArg, T2Args...> functor_two)
		: functor(functor)
		, functor_two(functor_two)
	{ }

	virtual TRet operator()(TBefore... before, T2Args... two_args, TAfter... after) override
	{
		if constexpr (IsErrorOr<TRet>::value && IsErrorOr<TArg>::value)
		{
			auto value = CHECK_RETURN(functor_two(two_args));
			return functor(before..., value, after...);
		}
		else if constexpr (IsErrorOr<TArg>::value)
		{
			static_assert(false, "Composed functor_two returns an ErrorOr but the functor does not, therefore we don't know how to pass through the error. Maybe consider wrapping functor_two in a default lambda");
		}
		else
		{
			return functor(before..., functor_two(two_args), after...);
		}
	}
}

// rmf note: I think I want another composed type for a shared parameter
// where the value gets used in both, rather than duplicated as a param
// to be used to generate an AST where each function also takes a context
// this is quite complicated with befores and afters, lets assume the shared context
// is the first parameter, for now
template <
	typename TRet,
	typename TShared,
	typename... T1Before,
	typename TArg,
	typename... T1After,
	typename... T2Args>
struct ComposedSharedParamFunctors : public Functor<TRet, TShared, T1Before...,  T2Args..., T1After...>
{
	value_ptr<Functor<TRet, TShared, TBefore..., UnwrapErrorOr<TArg>::TVal, TAfter...> > functor;
	value_ptr<Functor<TArg, TShared, T2Args...> > functor_two;

	ComposedSharedParamFunctors(
		Functor<TRet, TShared, TBefore..., UnwrapErrorOr<TArg>::TVal, TAfter...> functor,
		Functor<TArg, TShared, T2Args...> functor_two)
		: functor(functor)
		, functor_two(functor_two)
	{ }

	virtual TRet operator()(TShared shared, TBefore... before, T2Args... two_args, TAfter... after) override
	{
		if constexpr (IsErrorOr<TRet>::value && IsErrorOr<TArg>::value)
		{
			auto value = CHECK_RETURN(functor_two(shared, two_args));
			return functor(shared, before..., value, after...);
		}
		else if constexpr (IsErrorOr<TArg>::value)
		{
			static_assert(false, "ComposedSharedParam functor_two returns an ErrorOr but the functor does not, therefore we don't know how to pass through the error. Maybe consider wrapping functor_two in a default lambda");
		}
		else
		{
			return functor(shared, before..., functor_two(shared, two_args), after...);
		}
	}
}

template<typename TRet, typename ...TArgs>
struct FunctionPointer : public Functor<TRet, TArgs...>
{
	TRet (*func)(TArgs...);

	FunctionPointer(TRet (*func)(TArgs...))
		:func(func)
	{ }

	virtual TRet operator()(TArgs... args) override
	{
		return func(args...);
	}
};


// this lets you convert t.member(params) to func(t, params)
// what I'm really interested in is the other direction
// could use another operator (-> or | )
// see example at http://pfultz2.com/blog/2014/09/05/pipable-functions/
template <typename TRet, typename T, typename ...TArgs>
struct MemberFunction : public Functor<TRet, T&, TArgs...>
{
	TRet (T::*func)(TArgs...);

	MemberFunction(TRet (T::*func)(TArgs...))
		: func(func)
	{ }

	virtual TRet operator()(T& t, TArgs... args) override
	{
		return t.*func(args...);
	}
};


// rmf todo: how to specify partial specialization for containers?
// typename requires a fully resolved type
// can you template on a template name?
// maybe inference can just catch all this?
// type inference probably can't catch a return value...
template<template<typename, typename...> typename TContainer, typename TIn, typename TOut, typename ... TInArgs>
TContainer<TOut, TInArgs...> Map(
	const TContainer<TIn, TInArgs...>& in,
	Functor<TOut, const TIn &> & map)
{
	TContainer<TOut, TInArgs...> result{in.size()};
	for (const auto & val : in)
	{
		// rmf todo: which function to use here?
		// could we access by iterators in parallel?
		// could just pass in the function...
		result.push_back(map(val));
	}
}

template<template<typename, typename...> typename TContainer, typename TIn, typename ...TInArgs>
bool Apply(
	const TContainer<TIn, TInArgs...>& in,
	Functor<bool, TIn &> & apply,
	bool breakOnFailure = false)
{
	bool success = true;
	for (auto & val : in)
	{
		success = success && apply(val);
		if (breakOnFailure && !success)
		{
			return false;
		}
	}
	return success;
}

template<template<typename, typename...> typename TContainer, typename TIn, typename TOut, typename ... TInArgs>
TOut Reduce(
	const TContainer<TIn, TInArgs...> & in,
	Functor<TOut, const TOut &, const TIn &> & reduce,
	TOut initial = TOut{})
{
	TOut result = initial;
	for (const auto & val : in)
	{
		result = reduce(result, val);
	}
	return result;
}

template <typename TIn, typename TOut>
struct Sum : Functor<TOut, const TOut &, const TIn &>
{
	virtual TOut operator()(const TOut & aggregate, const TIn & nextValue) override
	{
		return aggregate + nextValue;
	}
};

template <typename TIn, typename TOut>
struct Max : Functor<TOut, const TOut &, const TIn &>
{
	virtual TOut operator()(const TOut & aggregate, const TIn & nextValue) override
	{
		static_assert(typeid(TIn) == typeid(TOut),
			"max requires in and out to be the same type");
		if (nextValue > aggregate)
		{
			// rmf note: do I need to static cast here?
			return nextValue;
		}
		return aggregate;
	}
};

template <typename TIn, typename TOut>
struct Min : Functor<TOut, const TOut &, const TIn &>
{
	virtual TOut operator()(const TOut & aggregate, const TIn & nextValue) override
	{
		static_assert(typeid(TIn) == typeid(TOut),
			"max requires in and out to be the same type");
		if (nextValue < aggregate)
		{
			return nextValue;
		}
		return aggregate;
	}
};

} // namespace Farb

#endif // FARB_MAP_REDUCE_HPP